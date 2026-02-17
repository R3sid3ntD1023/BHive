#include "VulkanPipeline.h"
#include "VulkanShader.h"
#include "VulkanVertexArray.h"
#include "VulkanRendererAPI.h"
#include "VulkanBuffers.h"
#include "VulkanSwapChain.h"
#include "VulkanConverters.h"
#include "gfx/BufferBase.h"

namespace BHive
{
	VulkanRendererAPI::VulkanRendererAPI()
		: mDevice(VulkanBackend::GetLogicalDevice())
	{

	}

	VulkanRendererAPI::~VulkanRendererAPI()
	{
	}

	void VulkanRendererAPI::Init()
	{
		auto graphics_queue_index = VulkanBackend::GetQueueFamilies().GraphicsQueueIndex;

		vk::CommandPoolCreateInfo pool_info( vk::CommandPoolCreateFlagBits::eResetCommandBuffer, graphics_queue_index);
		mCommandPool = vk::raii::CommandPool(mDevice, pool_info);

		vk::CommandBufferAllocateInfo alloc_info(mCommandPool, vk::CommandBufferLevel::ePrimary, VulkanBackend::MAX_FRAMES_IN_FLIGHT);
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
		LOG_TRACE("RendererAPI Shutdown Called")

		mCommandBuffers.clear();
		mCommandPool = VK_NULL_HANDLE;
		mDescriptorPool = VK_NULL_HANDLE;

		VulkanBackend::Get().Shutdown();
	}

	void VulkanRendererAPI::WaitIdle()
	{
		LOG_TRACE("RendererAPI Wait Idle")
		VulkanBackend::GetLogicalDevice().waitIdle();
	}

	vk::Result VulkanRendererAPI::RenderFrame(const Ref<VulkanSwapChain>& swapChain)
	{
		auto &command_buffer = mCommandBuffers[mCurrentFrame];
		auto &pre_commands = mCommands[ECommandType_PreCommand];
		auto &commands = mCommands[ECommandType_Command];
		FVulkanFrameData command_data{command_buffer, mCurrentFrame};
		ASSERT(mCurrentFrame < mCommandBuffers.size());

		
		swapChain->WaitForFence(mCurrentFrame);

		command_buffer.reset();

		auto [result, imageIndex] = swapChain->AquireNextImage(mCurrentFrame);
		auto &image = swapChain->GetImage(imageIndex);
		auto &depth_image = swapChain->GetDepthImage();
		auto extent = swapChain->GetExtent();

		if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
			return result;

		command_buffer.begin({});

		while (!pre_commands.empty())
		{
			auto &cmd = pre_commands.front();
			if (cmd)
				cmd(command_data);
			pre_commands.pop();
		}
		
		VulkanUtils::TransitionImageLayout(
			command_buffer, image.ImageSrc, image.Layout, vk::ImageLayout::eColorAttachmentOptimal, {}, vk::AccessFlagBits2::eColorAttachmentWrite,
			vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::ImageAspectFlagBits::eColor);

		VulkanUtils::TransitionImageLayout(
			command_buffer, depth_image.Image, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal, vk::AccessFlagBits2::eDepthStencilAttachmentWrite, vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
			vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
			vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests, vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil);

		image.Layout = vk::ImageLayout::eColorAttachmentOptimal;

		vk::ClearValue clearColor(mClearColor);
		vk::ClearValue clearDepth(vk::ClearDepthStencilValue(1.0f, 0));

		vk::RenderingAttachmentInfo attachmentInfo(
			image.View, vk::ImageLayout::eColorAttachmentOptimal, {}, {}, vk::ImageLayout::eUndefined, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
			clearColor);

		vk::RenderingAttachmentInfo depth_attachment_info(
			depth_image.View, vk::ImageLayout::eDepthStencilAttachmentOptimal, {}, {}, vk::ImageLayout::eUndefined, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare, clearDepth);

		vk::RenderingInfo renderingInfo({}, vk::Rect2D({0, 0}, extent), 1, 0, attachmentInfo, &depth_attachment_info);
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
			command_buffer, image.ImageSrc, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR, vk::AccessFlagBits2::eColorAttachmentWrite, {},
			vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::PipelineStageFlagBits2::eBottomOfPipe, vk::ImageAspectFlagBits::eColor);

		image.Layout = vk::ImageLayout::ePresentSrcKHR;

		command_buffer.end();

		result = swapChain->Present(command_buffer, imageIndex, mCurrentFrame);

		if (result != vk::Result::eSuccess)
			return result;

		mCurrentFrame = (mCurrentFrame + 1) % VulkanBackend::MAX_FRAMES_IN_FLIGHT;

		return vk::Result::eSuccess;
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

	void VulkanRendererAPI::Clear(ClearMask mask)
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
			data.CommandBuffer.setViewport(0, vk::Viewport((float)x, (float)(y + h), (float)w, -(float)h, 0.0f, 1.0f));
			data.CommandBuffer.setScissor(0, vk::Rect2D({(int32_t)x, (int32_t)y}, vk::Extent2D(w, h)));
		};

		SubmitCommand(cmd);
	}

	void VulkanRendererAPI::DrawArrays(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t count)
	{
		vao->Bind();

		auto topology = Vulkan::ToVkTopology(mode);

		auto cmd = [topology, count](const FVulkanFrameData &data)
		{
			data.CommandBuffer.setPrimitiveTopology(topology);
			data.CommandBuffer.draw(count, 1, 0, 0);
		};

		SubmitCommand(cmd);
	}

	void VulkanRendererAPI::DrawElements(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t count)
	{
		vao->Bind();
		auto index_buffer =vao->GetIndexBuffer();
		auto index_count = count ? count : index_buffer->GetCount();
		auto topology = Vulkan::ToVkTopology(mode);

		auto cmd = [topology, index_count](const FVulkanFrameData &data)
		{
			data.CommandBuffer.setPrimitiveTopology(topology);
			data.CommandBuffer.drawIndexed(index_count, 1, 0, 0, 0);
		};

		SubmitCommand(cmd);
	}

	void VulkanRendererAPI::DrawElementsBaseVertex(ETopologyMode mode, const VertexArray &vao, uint32_t start, uint32_t start_index, uint32_t count, uint32_t instance_count)
	{
		vao.Bind();
		auto index_buffer = vao.GetIndexBuffer();
		auto index_count = count ? count : index_buffer->GetCount();
		auto topology = Vulkan::ToVkTopology(mode);
	}

	void VulkanRendererAPI::DrawElementsRanged(ETopologyMode mode, const VertexArray &vao, uint32_t start, uint32_t end, uint32_t count)
	{
		vao.Bind();
		auto index_buffer = vao.GetIndexBuffer();

		auto _count = count ? count : index_buffer->GetCount();
	}

	void VulkanRendererAPI::DrawElementsInstanced(ETopologyMode mode, const VertexArray &vao, uint32_t instances, uint32_t count)
	{
		vao.Bind();
		auto index_buffer = vao.GetIndexBuffer();

		auto _count = count ? count : index_buffer->GetCount();
	}

	void VulkanRendererAPI::MultiDrawElementsIndirect(ETopologyMode mode, const BufferBase &indirect, const VertexArray &vao, size_t drawCount, size_t stride)
	{
		vao.Bind();

		auto buffer = indirect.GetNativeHandle().As<vk::Buffer>();
		auto topology = Vulkan::ToVkTopology(mode);

		auto cmd = [buffer, topology, drawCount, stride](const FVulkanFrameData &data)
		{		
			data.CommandBuffer.setPrimitiveTopology(topology);
			data.CommandBuffer.drawIndexedIndirect(*buffer, 0, drawCount, stride);
		};

		vao.UnBind();
	}

	void VulkanRendererAPI::EnableDepth()
	{

		
	}

	void VulkanRendererAPI::DisableDepth()
	{

		
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
