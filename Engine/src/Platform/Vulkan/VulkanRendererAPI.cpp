#include "core/Application.h"
#include "gfx/GraphicsContext.h"
#include "Platform/Vulkan/VulkanGraphicsContext.h"
#include "Platform/Vulkan/VulkanPipeline.h"
#include "Platform/Vulkan/VulkanShader.h"
#include "Platform/Vulkan/VulkanSwapChain.h"
#include "Platform/Vulkan/VulkanVertexArray.h"
#include "VulkanRendererAPI.h"
#include "VulkanUtils.h"
#include <glad/glad.h>
#include <glfw/glfw3.h>

namespace BHive
{
	namespace details
	{

		vk::PrimitiveTopology GetTopology(EDrawMode mode)
		{
			switch (mode)
			{
			case BHive::Lines:
				return vk::PrimitiveTopology::eLineList;
			default:
				break;
			}

			return vk::PrimitiveTopology::eTriangleList;
		}
	} // namespace details

	VulkanRendererAPI::~VulkanRendererAPI()
	{
	}

	void VulkanRendererAPI::BeginFrame()
	{
		auto &context = static_cast<VulkanGraphicsContext &>(GraphicsContext::Get());
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

	void VulkanRendererAPI::EndFrame()
	{
		auto &context = static_cast<VulkanGraphicsContext &>(GraphicsContext::Get());
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

	void VulkanRendererAPI::SubmitCommand(std::function<void(const FVulkanFrameData &)> &&command)
	{
		if (mDeviceRecreationInProgress.load())
			return;

		mCommands.push(FRenderCommand(std::move(command)));
	}

	void VulkanRendererAPI::SubmitSecondaryCommand(std::function<void(const FVulkanFrameData &)> &&command)
	{
		if (mDeviceRecreationInProgress.load())
			return;

		mSecondaryCommands.push(FRenderCommand(std::move(command)));
	}

	void VulkanRendererAPI::Init()
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
			});
	}

	void VulkanRendererAPI::Shutdown()
	{
	}

	vk::raii::CommandBuffer &VulkanRendererAPI::GetCurrentCommandBuffer()
	{
		auto &context = static_cast<VulkanGraphicsContext &>(GraphicsContext::Get());
		auto &swap_chain = context.GetSwapChain();
		auto current_frame = swap_chain->GetCurrentFrame();
		return mCommandBuffers[0].at(current_frame);
	}

	vk::raii::CommandBuffers *VulkanRendererAPI::AllocateCommandBuffers(uint32_t count)
	{
		auto &device = VulkanCore::GetLogicalDevice();

		vk::CommandBufferAllocateInfo alloc_info(mCommandPool, vk::CommandBufferLevel::ePrimary, count);
		mCommandBuffers.emplace_back(device, alloc_info);

		return &mCommandBuffers.back();
	}

	void VulkanRendererAPI::CreateCommandPool()
	{
		auto &device = VulkanCore::GetLogicalDevice();
		auto graphics_queue_index = VulkanCore::GetQueueFamilies().GraphicsQueueIndex;

		vk::CommandPoolCreateInfo pool_info;
		pool_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		pool_info.queueFamilyIndex = graphics_queue_index;

		mCommandPool = device.createCommandPool(pool_info);
	}

	void VulkanRendererAPI::CreateCommandBuffers()
	{
		AllocateCommandBuffers(2);
	}

	void VulkanRendererAPI::ClearColor(float r, float g, float b, float a)
	{
		mClearColor = {r, g, b, a};
	}

	void VulkanRendererAPI::Clear(int mask)
	{
	}

	void VulkanRendererAPI::SetLineWidth(float width)
	{
		auto cmd = [=](const FVulkanFrameData &data) { data.CommandBuffer.setLineWidth(width); };
		SubmitCommand(cmd);
	}

	void VulkanRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
	{
		auto cmd = [=](const FVulkanFrameData &data)
		{
			data.CommandBuffer.setViewport(0, vk::Viewport((float)x, (float)y, (float)w, (float)h, 0.0f, 1.0f));
			data.CommandBuffer.setScissor(0, vk::Rect2D({(int32_t)x, (int32_t)y}, {w, h}));

			LOG_TRACE("Set Viewport");
		};

		SubmitCommand(cmd);
	}

	void VulkanRendererAPI::DrawArrays(EDrawMode mode, const VertexArray &vao, uint32_t count)
	{
		vao.Bind();

		auto cmd = [=](const FVulkanFrameData &data)
		{
			data.CommandBuffer.setPrimitiveTopology(details::GetTopology(mode));
			data.CommandBuffer.draw(count, 1, 0, 0);

			LOG_TRACE("Draw Arrays");
		};

		SubmitCommand(cmd);
	}

	void VulkanRendererAPI::DrawElements(EDrawMode mode, const VertexArray &vao, uint32_t count)
	{
		vao.Bind();

		auto cmd = [=, &vao](const FVulkanFrameData &data)
		{
			auto index_buffer = vao.GetIndexBuffer();
			auto _count = count ? count : index_buffer->GetCount();

			data.CommandBuffer.setPrimitiveTopology(details::GetTopology(mode));
			data.CommandBuffer.drawIndexed(_count, 1, 0, 0, 0);
		};

		SubmitCommand(cmd);
	}

	void VulkanRendererAPI::DrawElementsBaseVertex(EDrawMode mode, const VertexArray &vao, uint32_t start, uint32_t start_index, uint32_t count, uint32_t instance_count)
	{
		vao.Bind();
		auto index_buffer = vao.GetIndexBuffer();

		auto _count = count ? count : index_buffer->GetCount();
	}

	void VulkanRendererAPI::DrawElementsRanged(EDrawMode mode, const VertexArray &vao, uint32_t start, uint32_t end, uint32_t count)
	{
		vao.Bind();
		auto index_buffer = vao.GetIndexBuffer();

		auto _count = count ? count : index_buffer->GetCount();
	}

	void VulkanRendererAPI::DrawElementsInstanced(EDrawMode mode, const VertexArray &vao, uint32_t instances, uint32_t count)
	{
		vao.Bind();
		auto index_buffer = vao.GetIndexBuffer();

		auto _count = count ? count : index_buffer->GetCount();
	}

	void VulkanRendererAPI::MultiDrawElementsIndirect(EDrawMode mode, const BufferBase &indirect, const VertexArray &vao, const void *data, size_t drawCount, size_t stride)
	{
		vao.Bind();

		vao.UnBind();
	}

	void VulkanRendererAPI::EnableDepth()
	{

		// glEnable(GL_DEPTH_TEST);
	}

	void VulkanRendererAPI::DisableDepth()
	{

		// glDisable(GL_DEPTH_TEST);
	}

	void VulkanRendererAPI::DepthFunc(uint32_t func)
	{

		// glDepthFunc(func);
	}

	void VulkanRendererAPI::CullFront()
	{

		// glCullFace(GL_FRONT);
	}

	void VulkanRendererAPI::CullBack()
	{

		// glCullFace(GL_BACK);
	}

	void VulkanRendererAPI::SetCullEnabled(bool enabled)
	{

		// enabled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
	}

	void VulkanRendererAPI::ColorMask(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		// glColorMask(r, g, b, a);
	}

	void VulkanRendererAPI::EnableDepthMask(bool mask)
	{

		// glDepthMask(mask ? GL_TRUE : GL_FALSE);
	}

	void VulkanRendererAPI::EnableBlend(bool enabled)
	{

		// enabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
	}

	void VulkanRendererAPI::AttachTextureToFramebuffer(uint32_t attachment, uint32_t texture, uint32_t framebuffer)
	{

		// glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture, framebuffer);
	}

} // namespace BHive
