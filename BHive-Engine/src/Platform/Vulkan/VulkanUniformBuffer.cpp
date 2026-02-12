#include "VulkanUniformBuffer.h"
#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"

namespace BHive
{
	VulkanUniformBuffer::VulkanUniformBuffer(uint32_t binding, uint64_t size, const void *data)
		: mDevice(VulkanBackend::GetLogicalDevice()),
		  mSize(size)
	{
		
		for (size_t i = 0; i < VulkanBackend::MAX_FRAMES_IN_FLIGHT; i++)
		{
			VulkanUtils::CreateBuffer(size, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent , mBuffer[i]);
			mMappedMemory[i] = mBuffer[i].Memory.mapMemory(0, size);
		}

		SetData(data, size, 0);	
	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		for (size_t i = 0; i < VulkanBackend::MAX_FRAMES_IN_FLIGHT; i++)
		{
			mMappedMemory[i] = nullptr;
			mBuffer[i].Memory.unmapMemory();
		}
	}

	void VulkanUniformBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		if (!data)
			return;

		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();
		auto cmd = [=](const FVulkanFrameData& frame)
			{		
				ASSERT(offset + size <= mSize);

				const auto current_frame = frame.Frame;
				std::memcpy(static_cast<std::byte*>(mMappedMemory[current_frame]) + offset, data, size);
			};
		
		api->SubmitCommand(cmd, ECommandType_PreCommand);
	}

	vk::DescriptorBufferInfo VulkanUniformBuffer::GetBufferInfo(uint32_t frame) const
	{
		ASSERT(frame < VulkanBackend::MAX_FRAMES_IN_FLIGHT);
		return vk::DescriptorBufferInfo(mBuffer[frame].Buffer, 0, mSize);
	}

} // namespace BHive