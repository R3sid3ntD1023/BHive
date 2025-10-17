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

		auto &command_buffer = mCommandBuffers[current_frame];

		command_buffer.begin({});

		VulkanUtils::TransitionImageLayout(
			command_buffer, image, image_index, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal, {}, vk::AccessFlagBits2::eColorAttachmentWrite, vk::PipelineStageFlagBits2::eTopOfPipe,
			vk::PipelineStageFlagBits2::eColorAttachmentOutput);

		vk::ClearValue clearColor = mClearColor;
		vk::RenderingAttachmentInfo attachmentInfo;
		attachmentInfo.imageView = image_view;
		attachmentInfo.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
		attachmentInfo.loadOp = vk::AttachmentLoadOp::eClear;
		attachmentInfo.storeOp = vk::AttachmentStoreOp::eStore;
		attachmentInfo.clearValue = clearColor;

		vk::RenderingInfo renderingInfo;
		renderingInfo.renderArea = vk::Rect2D({0, 0}, extent);
		renderingInfo.layerCount = 1;
		renderingInfo.colorAttachmentCount = 1;
		renderingInfo.pColorAttachments = &attachmentInfo;

		command_buffer.beginRendering(renderingInfo);
	}

	void RendererAPI::EndFrame()
	{
		auto &context = GraphicsContext::Get();
		auto &swap_chain = context.GetSwapChain();
		auto current_frame = swap_chain->GetCurrentFrame();
		auto image_index = context.GetImageIndex();

		auto &command_buffer = mCommandBuffers[current_frame];
		auto &image = swap_chain->GetImage(image_index);
		auto &image_view = swap_chain->GetImageView(image_index);

		FRenderCommand::FCommandData command_data{command_buffer, current_frame, image_index};

		while (!mCommands.empty())
		{
			auto& cmd = mCommands.front();
			cmd.Execute(command_data);
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
			cmd.Execute(command_data);
			mSecondaryCommands.pop();
		}
	}

	void RendererAPI::BindPipeline(const VulkanPipeline &pipeline)
	{
		mCommands.emplace(
			[&](const FRenderCommand::FCommandData& data)
			{
				data.CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
			});
	}

	void RendererAPI::BindDescriptorSets(const vk::raii::PipelineLayout &layout, const std::vector<vk::raii::DescriptorSet> &sets)
	{
		mCommands.emplace(
			[&](const FRenderCommand::FCommandData &data)
			{
				auto &cmd = GetCurrentCommandBuffer();
				data.CommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout, 0, *sets[data.Frame], nullptr);
			});
	}

	void RendererAPI::SubmitCommand(std::function<void(const FRenderCommand::FCommandData &)> &&command)
	{
		mCommands.push(FRenderCommand(std::move(command)));
	}

	void RendererAPI::SubmitSecondaryCommand(std::function<void(const FRenderCommand::FCommandData &)> &&command)
	{
		mSecondaryCommands.push(FRenderCommand(std::move(command)));
	}

	void RendererAPI::Init()
	{		
		CreateCommandPool();
		CreateCommandBuffers();
	}

	void RendererAPI::Shutdown()
	{
		auto &context = GraphicsContext::Get();
		auto &device = context.GetDevice();

		for (auto &module : mVulkanShaders)
			vkDestroyShaderModule(*device, module, nullptr);
	}

	vk::raii::CommandBuffer &RendererAPI::GetCurrentCommandBuffer()
	{
		auto &context = GraphicsContext::Get();
		auto &swap_chain = context.GetSwapChain();
		auto current_frame = swap_chain->GetCurrentFrame();
		return mCommandBuffers.at(current_frame);
	}

	void RendererAPI::SubmitCommandBuffer(const vk::CommandBuffer &cmdBuffer)
	{
		mAdditionalCommandBuffers.emplace(cmdBuffer);
	}

	void RendererAPI::CreateCommandPool()
	{
		auto &context = GraphicsContext::Get();
		auto &device = context.GetDevice();
		auto graphics_queue_index = context.GetQueueFamilies().GraphicsQueueIndex;

		vk::CommandPoolCreateInfo pool_info;
		pool_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		pool_info.queueFamilyIndex = graphics_queue_index;

		mCommandPool = device.createCommandPool(pool_info);
	}

	void RendererAPI::CreateCommandBuffers()
	{
		auto &context = GraphicsContext::Get();
		auto &device = context.GetDevice();

		vk::CommandBufferAllocateInfo alloc_info;
		alloc_info.commandPool = *mCommandPool;
		alloc_info.level = vk::CommandBufferLevel::ePrimary;
		alloc_info.commandBufferCount = 2;
		mCommandBuffers = vk::raii::CommandBuffers(device, alloc_info);
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
			[=](const FRenderCommand::FCommandData &data)
			{
				data.CommandBuffer.setViewport(0, vk::Viewport((float)x, (float)y, (float)w, (float)h, 0.0f, 1.0f));
				data.CommandBuffer.setScissor(0, vk::Rect2D({(int32_t)x, (int32_t)y }, {w, h}));
			});
		
	}

	void RendererAPI::DrawArrays(EDrawMode mode, const VertexArray &vao, uint32_t count)
	{
		vao.Bind();
	}

	void RendererAPI::DrawElements(EDrawMode mode, const VertexArray &vao, uint32_t count)
	{
		mCommands.emplace(
			[=](const FRenderCommand::FCommandData &data)
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
		//glDrawRangeElements(mode, start, end, _count, GL_UNSIGNED_INT, nullptr);
	}

	void RendererAPI::DrawElementsInstanced(EDrawMode mode, const VertexArray &vao, uint32_t instances, uint32_t count)
	{
		vao.Bind();
		auto index_buffer = vao.GetIndexBuffer();

		auto _count = count ? count : index_buffer->GetCount();
		//glDrawElementsInstanced(mode, _count, GL_UNSIGNED_INT, nullptr, instances);
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

		//glEnable(GL_DEPTH_TEST);
	}

	void RendererAPI::DisableDepth()
	{

		//glDisable(GL_DEPTH_TEST);
	}

	void RendererAPI::DepthFunc(uint32_t func)
	{

		//glDepthFunc(func);
	}

	void RendererAPI::CullFront()
	{

		//glCullFace(GL_FRONT);
	}

	void RendererAPI::CullBack()
	{

		//glCullFace(GL_BACK);
	}

	void RendererAPI::SetCullEnabled(bool enabled)
	{

		//enabled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
	}

	void RendererAPI::ColorMask(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		//glColorMask(r, g, b, a);
	}

	void RendererAPI::EnableDepthMask(bool mask)
	{

		//glDepthMask(mask ? GL_TRUE : GL_FALSE);
	}

	void RendererAPI::EnableBlend(bool enabled)
	{

		//enabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
	}

	void RendererAPI::AttachTextureToFramebuffer(uint32_t attachment, uint32_t texture, uint32_t framebuffer)
	{

		//glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture, framebuffer);
	}

	void *RendererAPI::CreateShader(const uint32_t *data, size_t size)
	{
		vk::ShaderModuleCreateInfo create_info({}, size, data);
		auto shader_module = VulkanUtils::CreateShaderModule(create_info);
		mVulkanShaders.push_back(shader_module);
		return &mVulkanShaders.back();
	}


} // namespace BHive
