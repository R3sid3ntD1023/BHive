#include "core/Application.h"
#include "GraphicsContext.h"
#include "RendererAPI.h"
#include "VulkanPipeline.h"
#include "VulkanSwapChain.h"
#include "VulkanUtils.h"
#include <glad/glad.h>
#include <glfw/glfw3.h>

namespace BHive
{

	RendererAPI::~RendererAPI()
	{
	}

	void RendererAPI::BeginFrame()
	{
		auto &context = GraphicsContext::Get();
		auto &swap_chain = context.GetSwapChain();

		auto current_frame = swap_chain->GetCurrentFrame();
		auto image_index = context.GetImageIndex();

		auto &image = swap_chain->GetImage(image_index);
		auto &image_view = swap_chain->GetImageView(image_index);
		auto extent = swap_chain->GetExtent();

		auto &command_buffer = mCommandBuffers[0][current_frame];

		vk::CommandBufferBeginInfo begin_info(vk::CommandBufferUsageFlagBits::eSimultaneousUse);
		command_buffer.begin(begin_info);

		VulkanUtils::TransitionImageLayout(
			command_buffer, image, image_index, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal, {}, vk::AccessFlagBits2::eColorAttachmentWrite,
			vk::PipelineStageFlagBits2::eTopOfPipe, vk::PipelineStageFlagBits2::eColorAttachmentOutput);

		vk::ClearValue clearColor = mClearColor;
		vk::RenderingAttachmentInfo attachmentInfo(
			image_view, vk::ImageLayout::eColorAttachmentOptimal, vk::ResolveModeFlagBits::eNone, {}, vk::ImageLayout::eUndefined, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
			clearColor);
		vk::RenderingInfo renderingInfo({}, vk::Rect2D({0, 0}, extent), 1, 0, attachmentInfo);
		command_buffer.beginRendering(renderingInfo);
	}

	void RendererAPI::EndFrame()
	{
		auto &context = GraphicsContext::Get();
		auto &swap_chain = context.GetSwapChain();
		auto current_frame = swap_chain->GetCurrentFrame();
		auto image_index = context.GetImageIndex();

		auto &command_buffer = mCommandBuffers[0][current_frame];
		auto &image = swap_chain->GetImage(image_index);
		auto &image_view = swap_chain->GetImageView(image_index);

		FVulkanFrameData command_data{command_buffer, image, image_view, current_frame};

		while (!mCommands.empty())
		{
			auto &cmd = mCommands.front();
			cmd(command_data);
			mCommands.pop();
		}

		command_buffer.endRendering();

		VulkanUtils::TransitionImageLayout(
			command_buffer, image, image_index, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR, vk::AccessFlagBits2::eColorAttachmentWrite, {},
			vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::PipelineStageFlagBits2::eBottomOfPipe);

		command_buffer.end();

		while (!mSecondaryCommands.empty())
		{
			auto &cmd = mSecondaryCommands.front();
			cmd(command_data);
			mSecondaryCommands.pop();
		}
	}

	void RendererAPI::BindPipeline(const VulkanPipeline &pipeline)
	{
		mCommands.emplace([&](const FVulkanFrameData &data) { data.CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline); });
	}

	void RendererAPI::BindDescriptorSets(const vk::raii::PipelineLayout &layout, const vk::raii::DescriptorSets &sets)
	{
		mCommands.emplace(
			[&](const FVulkanFrameData &data)
			{
				auto &cmd = GetCurrentCommandBuffer();
				data.CommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout, 0, *sets[data.Frame], nullptr);
			});
	}

	void RendererAPI::SubmitCommand(std::function<void(const FVulkanFrameData &)> &&command)
	{
		if (mDeviceRecreationInProgress.load())
			return;

		mCommands.push(FRenderCommand(std::move(command)));
	}

	void RendererAPI::SubmitSecondaryCommand(std::function<void(const FVulkanFrameData &)> &&command)
	{
		if (mDeviceRecreationInProgress.load())
			return;

		mSecondaryCommands.push(FRenderCommand(std::move(command)));
	}

	void RendererAPI::Init()
	{
		CreateCommandPool();
		CreateCommandBuffers();

		// called on device creation
		VulkanCore::RegisterOnDeviceCreated(
			[this]()
			{
				CreateCommandPool();
				CreateCommandBuffers();

				// allow subsequent commands to be submitted
				mDeviceRecreationInProgress.store(false);
			});

		// cleanup on device destroy
		VulkanCore::RegisterOnDeviceDestroy(
			[this]()
			{
				mDeviceRecreationInProgress.store(true);

				try
				{
					VulkanCore::GetLogicalDevice().waitIdle();
				}
				catch (...)
				{
				}

				mCommandBuffers.clear();

				mCommandPool = nullptr;

				try
				{
					for (auto &module : mVulkanShaders)
						vkDestroyShaderModule(*VulkanCore::GetLogicalDevice(), module, nullptr);
				}
				catch (...)
				{
				}

				mVulkanShaders.clear();
			});
	}

	void RendererAPI::Shutdown()
	{
	}

	vk::raii::CommandBuffer &RendererAPI::GetCurrentCommandBuffer()
	{
		auto &context = GraphicsContext::Get();
		auto &swap_chain = context.GetSwapChain();
		auto current_frame = swap_chain->GetCurrentFrame();
		return mCommandBuffers[0].at(current_frame);
	}

	vk::raii::CommandBuffers *RendererAPI::AllocateCommandBuffers(uint32_t count)
	{
		auto &device = VulkanCore::GetLogicalDevice();

		vk::CommandBufferAllocateInfo alloc_info(mCommandPool, vk::CommandBufferLevel::ePrimary, count);
		mCommandBuffers.emplace_back(device, alloc_info);

		return &mCommandBuffers.back();
	}

	void RendererAPI::CreateCommandPool()
	{
		auto &device = VulkanCore::GetLogicalDevice();
		auto graphics_queue_index = VulkanCore::GetQueueFamilies().GraphicsQueueIndex;

		vk::CommandPoolCreateInfo pool_info;
		pool_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		pool_info.queueFamilyIndex = graphics_queue_index;

		mCommandPool = device.createCommandPool(pool_info);
	}

	void RendererAPI::CreateCommandBuffers()
	{
		AllocateCommandBuffers(2);
	}

	void RendererAPI::ClearColor(float r, float g, float b, float a)
	{
		mClearColor = {r, g, b, a};
	}

	void RendererAPI::Clear(int mask)
	{
	}

	void RendererAPI::SetLineWidth(float width)
	{
	}

	void RendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
	{
		mCommands.emplace(
			[=](const FVulkanFrameData &data)
			{
				data.CommandBuffer.setViewport(0, vk::Viewport((float)x, (float)y, (float)w, (float)h, 0.0f, 1.0f));
				data.CommandBuffer.setScissor(0, vk::Rect2D({(int32_t)x, (int32_t)y}, {w, h}));
			});
	}

	void RendererAPI::DrawArrays(EDrawMode mode, const VertexArray &vao, uint32_t count)
	{
		mCommands.emplace(
			[=](const FVulkanFrameData &data)
			{
				std::vector<vk::Buffer> vk_vertex_buffers;

				auto vertex_buffers = vao.GetVertexBuffers();

				for (auto &vb : vertex_buffers)
				{
					const vk::raii::Buffer &vk_buffer = *vb;
					vk_vertex_buffers.push_back(*vk_buffer);
				}

				data.CommandBuffer.bindVertexBuffers(0, vk_vertex_buffers, {0});
				data.CommandBuffer.draw(count, 1, 0, 0);
			});
	}

	void RendererAPI::DrawElements(EDrawMode mode, const VertexArray &vao, uint32_t count)
	{
		mCommands.emplace(
			[=](const FVulkanFrameData &data)
			{
				std::vector<vk::Buffer> vk_vertex_buffers;

				auto index_buffer = vao.GetIndexBuffer();
				auto vertex_buffers = vao.GetVertexBuffers();

				for (auto &vb : vertex_buffers)
				{
					const vk::raii::Buffer &vk_buffer = *vb;
					vk_vertex_buffers.push_back(*vk_buffer);
				}

				const vk::raii::Buffer &vk_index_buffer = *index_buffer;

				auto _count = count ? count : index_buffer->GetCount();

				data.CommandBuffer.bindVertexBuffers(0, vk_vertex_buffers, {0});
				data.CommandBuffer.bindIndexBuffer(vk_index_buffer, 0, vk::IndexType::eUint32);
				data.CommandBuffer.drawIndexed(_count, 1, 0, 0, 0);
			});
	}

	void RendererAPI::DrawElementsBaseVertex(EDrawMode mode, const VertexArray &vao, uint32_t start, uint32_t start_index, uint32_t count, uint32_t instance_count)
	{
		vao.Bind();
		auto index_buffer = vao.GetIndexBuffer();

		auto _count = count ? count : index_buffer->GetCount();

		/*	if (instance_count > 0)
				glDrawElementsInstancedBaseVertexBaseInstance(mode, _count, GL_UNSIGNED_INT, nullptr, (GLsizei)instance_count, (GLint)start, 1);
			else
				glDrawElementsBaseVertex(mode, _count, GL_UNSIGNED_INT, nullptr, start);*/
	}

	void RendererAPI::DrawElementsRanged(EDrawMode mode, const VertexArray &vao, uint32_t start, uint32_t end, uint32_t count)
	{
		vao.Bind();
		auto index_buffer = vao.GetIndexBuffer();

		auto _count = count ? count : index_buffer->GetCount();
		// glDrawRangeElements(mode, start, end, _count, GL_UNSIGNED_INT, nullptr);
	}

	void RendererAPI::DrawElementsInstanced(EDrawMode mode, const VertexArray &vao, uint32_t instances, uint32_t count)
	{
		vao.Bind();
		auto index_buffer = vao.GetIndexBuffer();

		auto _count = count ? count : index_buffer->GetCount();
		// glDrawElementsInstanced(mode, _count, GL_UNSIGNED_INT, nullptr, instances);
	}

	void RendererAPI::MultiDrawElementsIndirect(EDrawMode mode, const BufferBase &indirect, const VertexArray &vao, const void *data, size_t drawCount, size_t stride)
	{
		vao.Bind();

		/*glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirect.GetBufferID());

		glMultiDrawElementsIndirect(mode, GL_UNSIGNED_INT, nullptr, drawCount, stride);

		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);*/

		vao.UnBind();
	}

	void RendererAPI::EnableDepth()
	{

		// glEnable(GL_DEPTH_TEST);
	}

	void RendererAPI::DisableDepth()
	{

		// glDisable(GL_DEPTH_TEST);
	}

	void RendererAPI::DepthFunc(uint32_t func)
	{

		// glDepthFunc(func);
	}

	void RendererAPI::CullFront()
	{

		// glCullFace(GL_FRONT);
	}

	void RendererAPI::CullBack()
	{

		// glCullFace(GL_BACK);
	}

	void RendererAPI::SetCullEnabled(bool enabled)
	{

		// enabled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
	}

	void RendererAPI::ColorMask(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		// glColorMask(r, g, b, a);
	}

	void RendererAPI::EnableDepthMask(bool mask)
	{

		// glDepthMask(mask ? GL_TRUE : GL_FALSE);
	}

	void RendererAPI::EnableBlend(bool enabled)
	{

		// enabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
	}

	void RendererAPI::AttachTextureToFramebuffer(uint32_t attachment, uint32_t texture, uint32_t framebuffer)
	{

		// glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture, framebuffer);
	}

	void *RendererAPI::CreateShader(const uint32_t *data, size_t size)
	{
		vk::ShaderModuleCreateInfo create_info({}, size, data);
		auto shader_module = VulkanUtils::CreateShaderModule(create_info);
		mVulkanShaders.push_back(shader_module);
		return &mVulkanShaders.back();
	}

} // namespace BHive
