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
		VulkanUtils::CreateBuffer(count * sizeof(uint32_t), vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal, mBuffer);
		SetData(data, count * sizeof(uint32_t));

	}

	void VulkanIndexBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		if (!data)
			return;

		AllocatedVulkanBuffer stagingBuffer{};

		VulkanUtils::CreateBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer);

		stagingBuffer.SetData(data, size, offset);

		VulkanUtils::CopyBuffer(stagingBuffer, mBuffer, size);
	}

	VulkanVertexBuffer::VulkanVertexBuffer(const size_t size, const void *data)
		: mDevice(VulkanCore::GetLogicalDevice())
	{
		for (uint32_t i = 0; i < 2; i++)
			VulkanUtils::CreateBuffer(size, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal, mBuffer[i]);

		SetData(data, size, 0);
	}

	void VulkanVertexBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		if (!data)
			return;

		auto ownedData = CreateRef<std::vector<std::byte>>();
		ownedData->resize(size);
		std::memcpy(ownedData->data(), data, size);

		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();
		auto cmd = [=](const FVulkanFrameData& frame)
			{
				AllocatedVulkanBuffer stagingBuffer{};

				VulkanUtils::CreateBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer);

				stagingBuffer.SetData(ownedData->data(), size, offset);

				auto& dst_buffer = mBuffer[frame.Frame].Buffer;
				vk::BufferCopy copy_region(0, offset, size);
				frame.CommandBuffer.copyBuffer(stagingBuffer.Buffer, dst_buffer, copy_region);

				api->GetFrameResources(frame.Frame).StagingBuffers.emplace_back(std::move(stagingBuffer));

				vk::BufferMemoryBarrier2 barrier(
				vk::PipelineStageFlagBits2::eCopy, vk::AccessFlagBits2::eTransferWrite, vk::PipelineStageFlagBits2::eVertexAttributeInput, vk::AccessFlagBits2::eVertexAttributeRead,
				VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, dst_buffer, 0, VK_WHOLE_SIZE);

				vk::DependencyInfo dependency_info({}, {}, barrier);

				frame.CommandBuffer.pipelineBarrier2(dependency_info);
			};
		
		api->SubmitCommand(cmd, ECommandType_PreCommand);
	}

	void VulkanVertexBuffer::SetLayout(const BufferLayout &layout)
	{
		mLayout = layout;
	}

} // namespace BHive