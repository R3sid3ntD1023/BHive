#include "VulkanBuffers.h"
#include "VulkanUtils.h"
#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"

namespace BHive
{

	VulkanIndexBuffer::VulkanIndexBuffer(const uint32_t count, const uint32_t *data)
		: mDevice(VulkanCore::GetLogicalDevice()),
		  mCount(count)
	{
		auto size = count * sizeof(uint32_t);
		
		mBuffer.Init(size, vk::BufferUsageFlagBits::eIndexBuffer);

		if (data && size)
			SetData(data, size, 0);
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		mBuffer.Release();
	}

	void VulkanIndexBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		if (!data || size == 0)
			return;

		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();
		auto cmd = [=](const FVulkanFrameData &frame)
		{
			auto cmd = [this, data, size, offset](const FVulkanFrameData &frame)
			{
				mBuffer.SetData(frame.CommandBuffer, data, size, offset, vk::PipelineStageFlagBits2::eIndexInput, vk::AccessFlagBits2::eIndexRead);
			};

			api->SubmitCommand(cmd, ECommandType_PreCommand);
		};

		api->SubmitCommand(cmd, ECommandType_PreCommand);
	}

	VulkanVertexBuffer::VulkanVertexBuffer(const size_t size, const void *data)
		: mDevice(VulkanCore::GetLogicalDevice())
	{
		for (uint32_t i = 0; i < VulkanCore::MAX_FRAMES_IN_FLIGHT; i++)
		{
			mPerFrameBuffer[i].Init(size, vk::BufferUsageFlagBits::eVertexBuffer);	
		}

		if (data && size)
			SetData(data, size, 0);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		for (uint32_t i = 0; i < VulkanCore::MAX_FRAMES_IN_FLIGHT; i++)
		{
			mPerFrameBuffer[i].Release();
		}
	}

	void VulkanVertexBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		if (!data || size == 0)
			return;

		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();

		auto cmd = [this, data, size, offset](const FVulkanFrameData &frame)
		{
			for (uint32_t i = 0; i < VulkanCore::MAX_FRAMES_IN_FLIGHT; i++)
			{
				auto &current_frame_buffer = mPerFrameBuffer[i];
				current_frame_buffer.SetData(frame.CommandBuffer, data, size, offset, vk::PipelineStageFlagBits2::eVertexAttributeInput, vk::AccessFlagBits2::eVertexAttributeRead);
			}
		};

		api->SubmitCommand(cmd, ECommandType_PreCommand);
	}

	void VulkanVertexBuffer::SetLayout(const BufferLayout &layout)
	{
		mLayout = layout;
	}

	void PerFrameBuffer::Init(size_t size, vk::BufferUsageFlags usage)
	{
		VulkanUtils::CreateBuffer(size, usage | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal, Buffer);
		VulkanUtils::CreateBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, StagingBuffer);
		MappedMemory = StagingBuffer.Memory.mapMemory(0, size);
	}

	void PerFrameBuffer::SetData(vk::raii::CommandBuffer &cmd, const void *data, size_t size, uint32_t offset, vk::PipelineStageFlags2 flags, vk::AccessFlags2 access)
	{
		if (MappedMemory)
		{
			std::memcpy(static_cast<std::byte *>(MappedMemory) + offset, data, size);
		}

		vk::BufferCopy copy_region(0, offset, size);
		cmd.copyBuffer(StagingBuffer.Buffer, Buffer.Buffer, copy_region);

		vk::BufferMemoryBarrier2 barrier(
			vk::PipelineStageFlagBits2::eCopy, vk::AccessFlagBits2::eTransferWrite, flags, access,
			VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, Buffer.Buffer, 0, size);

		vk::DependencyInfo dependency_info({}, {}, barrier);

		cmd.pipelineBarrier2(dependency_info);
	}

	void PerFrameBuffer::Release()
	{
		MappedMemory = nullptr;
		StagingBuffer.Memory.unmapMemory();
	}

} // namespace BHive