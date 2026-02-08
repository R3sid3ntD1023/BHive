#include "VulkanPipeline.h"
#include "VulkanShader.h"
#include "VulkanVertexArray.h"
#include "VulkanRendererAPI.h"
#include "VulkanBuffers.h"

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

		vk::CommandPoolCreateInfo pool_info( vk::CommandPoolCreateFlagBits::eResetCommandBuffer, graphics_queue_index);
		mCommandPool = vk::raii::CommandPool(mDevice, pool_info);

		vk::CommandBufferAllocateInfo alloc_info(mCommandPool, vk::CommandBufferLevel::ePrimary, VulkanCore::MAX_FRAMES_IN_FLIGHT);
		mCommandBuffers = vk::raii::CommandBuffers(mDevice, alloc_info);

		const uint32_t descriptor_count = 1000;

		std::vector<vk::DescriptorPoolSize> pool_sizes;
		pool_sizes.emplace_back(vk::DescriptorType::eSampler, descriptor_count);
		pool_sizes.emplace_back(vk::DescriptorType::eSampledImage, descriptor_count);
		pool_sizes.emplace_back(vk::DescriptorType::eCombinedImageSampler, descriptor_count);
		pool_sizes.emplace_back(vk::DescriptorType::eStorageImage, descriptor_count);
		pool_sizes.emplace_back(vk::DescriptorType::eUniformBuffer, descriptor_count);
		pool_sizes.emplace_back(vk::DescriptorType::eUniformBufferDynamic, descriptor_count);
		pool_sizes.emplace_back(vk::DescriptorType::eUniformTexelBuffer, descriptor_count);
		pool_sizes.emplace_back(vk::DescriptorType::eStorageBuffer, descriptor_count);
		pool_sizes.emplace_back(vk::DescriptorType::eStorageBufferDynamic, descriptor_count);
		pool_sizes.emplace_back(vk::DescriptorType::eStorageTexelBuffer, descriptor_count);
		pool_sizes.emplace_back(vk::DescriptorType::eInputAttachment, descriptor_count);

		vk::DescriptorPoolCreateInfo pool_create_info(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, 1000, pool_sizes);
		mDescriptorPool = mDevice.createDescriptorPool(pool_create_info);

	}

	void VulkanRendererAPI::Shutdown()
	{

	}

	vk::raii::CommandBuffer &VulkanRendererAPI::RenderFrame(uint32_t frame, vk::ImageLayout& layout, vk::Image &image, vk::raii::ImageView &image_view, const vk::Extent2D &extent)
	{
		auto &command_buffer = mCommandBuffers[frame];
		auto &pre_commands = mCommands[ECommandType_PreCommand];
		auto &commands = mCommands[ECommandType_Command];
		FVulkanFrameData command_data{command_buffer, frame};
		ASSERT(frame < mCommandBuffers.size());

		command_buffer.reset();
		command_buffer.begin({});

		while (!pre_commands.empty())
		{
			auto &cmd = pre_commands.front();
			if (cmd)
				cmd(command_data);
			pre_commands.pop();
		}
		
		VulkanUtils::TransitionImageLayout(
			command_buffer, image, layout, vk::ImageLayout::eColorAttachmentOptimal, {}, vk::AccessFlagBits2::eColorAttachmentWrite,
			vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::PipelineStageFlagBits2::eColorAttachmentOutput);

		layout = vk::ImageLayout::eColorAttachmentOptimal;

		vk::ClearValue clearColor = mClearColor;
		vk::RenderingAttachmentInfo attachmentInfo(
			image_view, vk::ImageLayout::eColorAttachmentOptimal, vk::ResolveModeFlagBits::eNone, {}, vk::ImageLayout::eUndefined, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
			clearColor);
		vk::RenderingInfo renderingInfo({}, vk::Rect2D({0, 0}, extent), 1, 0, attachmentInfo);
		command_buffer.beginRendering(renderingInfo);

		
		while (!commands.empty())
		{
			auto &cmd = commands.front();
			if (cmd)
				cmd(command_data);
			commands.pop();
		}

		command_buffer.endRendering();

		VulkanUtils::TransitionImageLayout(
			command_buffer, image, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR, vk::AccessFlagBits2::eColorAttachmentWrite, {},
			vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::PipelineStageFlagBits2::eBottomOfPipe);

		layout = vk::ImageLayout::ePresentSrcKHR;

		command_buffer.end();

		return command_buffer;
	}

	void VulkanRendererAPI::SubmitCommand(const FRenderCommand &command, ECommandType type)
	{
		if (mDeviceRecreationInProgress.load())
		{
			LOG_TRACE("Device Recreation in Progress");
			return;
		}

		mCommands[type].emplace(command);
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
			//LOG_INFO("CMD: SetViewport, frame={}", data.Frame);
			data.CommandBuffer.setViewport(0, vk::Viewport((float)x, (float)y, (float)w, (float)h, 0.0f, 1.0f));
			data.CommandBuffer.setScissor(0, vk::Rect2D({(int32_t)x, (int32_t)y}, vk::Extent2D(w, h)));
		};

		SubmitCommand(cmd);
	}

	void VulkanRendererAPI::DrawArrays(EDrawMode mode, const Ref<VertexArray> &vao, uint32_t count)
	{
		vao->Bind();

		auto topology = details::GetTopology(mode);

		auto cmd = [topology, count](const FVulkanFrameData &data)
		{
			data.CommandBuffer.setPrimitiveTopology(topology);
			data.CommandBuffer.draw(count, 1, 0, 0);
		};

		SubmitCommand(cmd);
	}

	void VulkanRendererAPI::DrawElements(EDrawMode mode, const Ref<VertexArray> &vao, uint32_t count)
	{
		vao->Bind();
		auto index_buffer =vao->GetIndexBuffer();
		auto index_count = count ? count : index_buffer->GetCount();
		auto topology = details::GetTopology(mode);

		auto cmd = [topology, index_count](const FVulkanFrameData &data)
		{
			data.CommandBuffer.setPrimitiveTopology(topology);
			data.CommandBuffer.drawIndexed(index_count, 1, 0, 0, 0);
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
