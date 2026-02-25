#include "VulkanPipeline.h"
#include "VulkanShader.h"
#include "VulkanVertexArray.h"
#include "VulkanRendererAPI.h"
#include "VulkanBuffers.h"
#include "VulkanSwapChain.h"
#include "VulkanConverters.h"
#include "gfx/BufferBase.h"
#include "VulkanFramebuffer.h"
#include "core/Window.h"
#include "VulkanWindowContext.h"
#include "gfx/RenderCommand.h"
#include "IVulkanTexture.h"
#include "GPUResourceManager.h"

namespace BHive
{
	struct PendingDeletion
	{
		uint32_t Frame = 0;
		std::function<void(uint32_t)> Fn;
	};

	static std::queue<PendingDeletion> sDeletionQueue;

	VulkanRendererAPI::VulkanRendererAPI()
		: mDevice(VulkanBackend::GetLogicalDevice())
	{

	}

	VulkanRendererAPI::~VulkanRendererAPI()
	{
		
	}

	void VulkanRendererAPI::Init()
	{
		

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

		vk::DescriptorPoolCreateInfo pool_create_info(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet | vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind, 1000, pool_sizes);
		mDescriptorPool = mDevice.createDescriptorPool(pool_create_info);

	}

	void VulkanRendererAPI::Shutdown()
	{
		LOG_TRACE("RendererAPI Shutdown Called")

		mDescriptorPool = VK_NULL_HANDLE;

		GPUResourceManager::Get().Shutdown();

		VulkanBackend::Get().Shutdown();
	}

	void VulkanRendererAPI::WaitIdle()
	{
		LOG_TRACE("RendererAPI Wait Idle")
		VulkanBackend::GetLogicalDevice().waitIdle();
	}

	vk::Result VulkanRendererAPI::RenderFrame(VulkanWindowContext *ctx)
	{
		auto current_frame = ctx->GetCurrentFrame();
		auto &cmd = ctx->GetCommandBuffer();

		auto &pre_commands = mCommands[ECommandType_PreCommand];
		auto &commands = mCommands[ECommandType_Command];
		auto &screen_commands = mCommands[ECommandType_ToScreen];
		
		ProcessDeletionQueue(current_frame);

		auto swap_chain = ctx->GetSwapChain();

		swap_chain->WaitForFence(current_frame);

		cmd.reset();

		auto [result, imageIndex] = swap_chain->AquireNextImage(current_frame);

		auto &image = swap_chain->GetImage(imageIndex);
		auto &depth = swap_chain->GetDepthImage();
		auto extent = swap_chain->GetExtent();

		if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
			return result;

		cmd.begin({});
		
		FVulkanFrame frame{cmd, current_frame, imageIndex};
		
		while (!pre_commands.empty())
		{
			auto &cmd = pre_commands.front();
			if (cmd)
				cmd(frame);
			pre_commands.pop();
		}

		while (!commands.empty())
		{
			auto &cmd = commands.front();
			if (cmd)
				cmd(frame);
			commands.pop();
		}


		Vulkan::ImageState colorAttachmentState = {vk::ImageLayout::eColorAttachmentOptimal, vk::AccessFlagBits2::eColorAttachmentWrite, vk::PipelineStageFlagBits2::eColorAttachmentOutput};
		Vulkan::ImageState depthAttachmentState = {
			vk::ImageLayout::eDepthStencilAttachmentOptimal, vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
			vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests};

		image.Transition(frame.CommandBuffer, colorAttachmentState );
		depth.Transition(frame.CommandBuffer, depthAttachmentState);

		vk::ClearValue clearColor(mClearColor);
		vk::ClearValue clearDepth(vk::ClearDepthStencilValue(1.0f, 0));

		vk::RenderingAttachmentInfo attachmentInfo(
			image.GetView(), vk::ImageLayout::eColorAttachmentOptimal, {}, {}, vk::ImageLayout::eUndefined, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, clearColor);

		vk::RenderingAttachmentInfo depth_attachment_info(
			depth.GetView(), vk::ImageLayout::eDepthStencilAttachmentOptimal, {}, {}, vk::ImageLayout::eUndefined, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare, clearDepth);

		vk::RenderingInfo renderingInfo({}, vk::Rect2D({0, 0}, extent), 1, 0, attachmentInfo, &depth_attachment_info);
		frame.CommandBuffer.beginRendering(renderingInfo);

		while (!screen_commands.empty())
		{
			auto &cmd = screen_commands.front();
			if (cmd)
				cmd(frame);
			screen_commands.pop();
		}

		frame.CommandBuffer.endRendering();

		Vulkan::ImageState present = {vk::ImageLayout::ePresentSrcKHR, {}, vk::PipelineStageFlagBits2::eBottomOfPipe};
		image.Transition(frame.CommandBuffer, present);

		cmd.end();

		result = swap_chain->Present(cmd, imageIndex, current_frame);

		mCompletedFrame = current_frame;
		
		if (result != vk::Result::eSuccess)
			return result;

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

	void VulkanRendererAPI::QueueDeletion(std::function<void(uint32_t)> fn)
	{
		sDeletionQueue.emplace(mCompletedFrame, fn);
	}

	void VulkanRendererAPI::ProcessDeletionQueue(uint32_t frame)
	{
		while (!sDeletionQueue.empty())
		{
			auto & del = sDeletionQueue.front();

			if (frame < del.Frame)
				del.Fn(frame);
			sDeletionQueue.pop();
		}
	}

	void VulkanRendererAPI::SetCurrentContext(VulkanWindowContext *ctx)
	{
		mCurrentContext = ctx;
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
		auto cmd = [=](const FVulkanFrame &data) { data.CommandBuffer.setLineWidth(width); };
		SubmitCommand(cmd);
	}

	void VulkanRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
	{
		auto cmd = [=](const FVulkanFrame &data)
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

		auto cmd = [topology, count](const FVulkanFrame &data)
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

		auto cmd = [topology, index_count](const FVulkanFrame &data)
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

		auto cmd = [buffer, topology, drawCount, stride](const FVulkanFrame &data)
		{		
			data.CommandBuffer.setPrimitiveTopology(topology);
			data.CommandBuffer.drawIndexedIndirect(*buffer, 0, drawCount, stride);
		};

		vao.UnBind();
	}

	void VulkanRendererAPI::Dispath(uint32_t x, uint32_t y, uint32_t z)
	{
		auto cmd = [x, y, z](const FVulkanFrame &data)
		{
			data.CommandBuffer.dispatch(x, y, z);
		};

		SubmitCommand(cmd);
	}

	void VulkanRendererAPI::ColorMask(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{	
	}

} // namespace BHive
