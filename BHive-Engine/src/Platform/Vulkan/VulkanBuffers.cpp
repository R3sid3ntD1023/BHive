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
		VulkanUtils::CreateBuffer(size, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal, mBuffer);
		VulkanUtils::CreateBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, mStagingBuffer);
		mMappedMemory = mStagingBuffer.Memory.mapMemory(0, size);

		if (data && size)
			SetData(data, size, 0);
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		mMappedMemory = nullptr;
		mStagingBuffer.Memory.unmapMemory();
	}

	void VulkanIndexBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		if (!data)
			return;

		if (!data || size == 0)
			return;

		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();

		auto ownedData = CreateRef<std::vector<std::byte>>(size);
		std::memcpy(ownedData->data(), data, size);

		auto cmd = [=](const FVulkanFrameData &frame)
		{
			if (mMappedMemory)
			{
				std::memcpy(static_cast<std::byte *>(mMappedMemory) + offset, ownedData->data(), size);
			}

			vk::BufferCopy copy_region(0, offset, size);
			frame.CommandBuffer.copyBuffer(mStagingBuffer.Buffer, mBuffer.Buffer, copy_region);

			vk::BufferMemoryBarrier2 barrier(
				vk::PipelineStageFlagBits2::eCopy, vk::AccessFlagBits2::eTransferWrite, vk::PipelineStageFlagBits2::eIndexInput, vk::AccessFlagBits2::eIndexRead,
				VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, mBuffer.Buffer, 0, size);

			vk::DependencyInfo dependency_info({}, {}, barrier);

			frame.CommandBuffer.pipelineBarrier2(dependency_info);
		};

		api->SubmitCommand(cmd, ECommandType_PreCommand);
	}

	VulkanVertexBuffer::VulkanVertexBuffer(const size_t size, const void *data)
		: mDevice(VulkanCore::GetLogicalDevice())
	{
		for (uint32_t i = 0; i < VulkanCore::MAX_FRAMES_IN_FLIGHT; i++)
		{
			mPerFrameBuffer[i].Init(size);
			
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

		auto ownedData = CreateRef<std::vector<std::byte>>(size);
		std::memcpy(ownedData->data(), data, size);

		auto cmd = [this, ownedData, size, offset](const FVulkanFrameData &frame)
		{
			auto &current_frame_buffer = mPerFrameBuffer[frame.Frame];
			current_frame_buffer.SetData(frame.CommandBuffer, ownedData->data(), size, offset);
		};

		api->SubmitCommand(cmd, ECommandType_PreCommand);
	}

	void VulkanVertexBuffer::SetLayout(const BufferLayout &layout)
	{
		mLayout = layout;
	}

	void VulkanVertexBuffer::PerFrameBuffer::Init(size_t size)
	{
		VulkanUtils::CreateBuffer(size, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal, Buffer);
		VulkanUtils::CreateBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, StagingBuffer);
		MappedMemory = StagingBuffer.Memory.mapMemory(0, size);
	}

	void VulkanVertexBuffer::PerFrameBuffer::SetData(vk::raii::CommandBuffer &cmd, const void *data, size_t size, uint32_t offset)
	{
		if (MappedMemory)
		{
			std::memcpy(static_cast<std::byte *>(MappedMemory) + offset, data, size);
		}

		vk::BufferCopy copy_region(0, offset, size);
		cmd.copyBuffer(StagingBuffer.Buffer, Buffer.Buffer, copy_region);

		vk::BufferMemoryBarrier2 barrier(
			vk::PipelineStageFlagBits2::eCopy, vk::AccessFlagBits2::eTransferWrite, vk::PipelineStageFlagBits2::eVertexAttributeInput, vk::AccessFlagBits2::eVertexAttributeRead,
			VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, Buffer.Buffer, 0, size);

		vk::DependencyInfo dependency_info({}, {}, barrier);

		cmd.pipelineBarrier2(dependency_info);
	}

	void VulkanVertexBuffer::PerFrameBuffer::Release()
	{
		MappedMemory = nullptr;
		StagingBuffer.Memory.unmapMemory();
	}

} // namespace BHive