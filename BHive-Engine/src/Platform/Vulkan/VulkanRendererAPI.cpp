#include "Platform/Vulkan/VulkanPipeline.h"
#include "Platform/Vulkan/VulkanShader.h"
#include "Platform/Vulkan/VulkanVertexArray.h"
#include "VulkanRendererAPI.h"

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

	VulkanRendererAPI::VulkanRendererAPI()
		: mDevice(VulkanCore::GetLogicalDevice())
	{
	}

	VulkanRendererAPI::~VulkanRendererAPI()
	{
	}

	void VulkanRendererAPI::Init()
	{
		auto graphics_queue_index = VulkanCore::GetQueueFamilies().GraphicsQueueIndex;

		vk::CommandPoolCreateInfo pool_info;
		pool_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		pool_info.queueFamilyIndex = graphics_queue_index;

		mCommandPool = mDevice.createCommandPool(pool_info);

		vk::CommandBufferAllocateInfo alloc_info(mCommandPool, vk::CommandBufferLevel::ePrimary, 2);
		mCommandBuffers = vk::raii::CommandBuffers(mDevice, alloc_info);
	}

	void VulkanRendererAPI::Shutdown()
	{

	}

	vk::raii::CommandBuffer& VulkanRendererAPI::RenderFrame(uint32_t frame, uint32_t imageIndex, vk::Image &image, vk::raii::ImageView &image_view, const vk::Extent2D &extent)
	{
		auto &command_buffer = mCommandBuffers[frame];

		command_buffer.reset();

		vk::CommandBufferBeginInfo begin_info(vk::CommandBufferUsageFlagBits::eSimultaneousUse);
		command_buffer.begin(begin_info);
		
		VulkanUtils::TransitionImageLayout(
			command_buffer, image, imageIndex, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal, {}, vk::AccessFlagBits2::eColorAttachmentWrite,
			vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::PipelineStageFlagBits2::eColorAttachmentOutput);

		vk::ClearValue clearColor = mClearColor;
		vk::RenderingAttachmentInfo attachmentInfo(
			image_view, vk::ImageLayout::eColorAttachmentOptimal, vk::ResolveModeFlagBits::eNone, {}, vk::ImageLayout::eUndefined, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
			clearColor);
		vk::RenderingInfo renderingInfo({}, vk::Rect2D({0, 0}, extent), 1, 0, attachmentInfo);
		command_buffer.beginRendering(renderingInfo);

		FVulkanFrameData command_data{command_buffer, image, image_view, frame};

		while (!mCommands.empty())
		{
			auto &cmd = mCommands.front();
			if (cmd)
				cmd(command_data);
			mCommands.pop();
		}

		command_buffer.endRendering();

		VulkanUtils::TransitionImageLayout(
			command_buffer, image, imageIndex, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR, vk::AccessFlagBits2::eColorAttachmentWrite, {},
			vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::PipelineStageFlagBits2::eBottomOfPipe);

		command_buffer.end();

		return command_buffer;
	}

	void VulkanRendererAPI::SubmitCommand(const FRenderCommand &command)
	{
		if (mDeviceRecreationInProgress.load())
		{
			LOG_TRACE("Device Recreation in Progress");
			return;
		}

		mCommands.emplace(command);
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
			data.CommandBuffer.setScissor(0, vk::Rect2D({(int32_t)x, (int32_t)y}, vk::Extent2D(w, h)));
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
	}

	void VulkanRendererAPI::CullFront()
	{	
	}

	void VulkanRendererAPI::CullBack()
	{	
	}

	void VulkanRendererAPI::SetCullEnabled(bool enabled)
	{	
	}

	void VulkanRendererAPI::ColorMask(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{	
	}

	void VulkanRendererAPI::EnableDepthMask(bool mask)
	{
	}

	void VulkanRendererAPI::EnableBlend(bool enabled)
	{	
	}

	void VulkanRendererAPI::AttachTextureToFramebuffer(uint32_t attachment, uint32_t texture, uint32_t framebuffer)
	{	
	}

	

} // namespace BHive
