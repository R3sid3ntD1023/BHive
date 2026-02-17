#include "VulkanStorageBuffer.h"
#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"

namespace BHive
{

	VulkanStorageBuffer::VulkanStorageBuffer(uint32_t binding, size_t size, const void *data)
		: mDevice(VulkanBackend::GetLogicalDevice()),
		  mBinding(binding),
		  mSize(size)
	{
		for (uint32_t i = 0; i < VulkanBackend::MAX_FRAMES_IN_FLIGHT; i++)
		{
			VulkanUtils::CreateBuffer(size, vk::BufferUsageFlagBits::eStorageBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, mBuffer[i]);

			SetData(data, size, 0);

			mMappedMemory[i] = mBuffer[i].Memory.mapMemory(0, size);

			mBufferInfo[i] = vk::DescriptorBufferInfo(mBuffer[i].Buffer, 0, mSize);
		}

		
	}

	VulkanStorageBuffer::VulkanStorageBuffer(size_t size)
		: mDevice(VulkanBackend::GetLogicalDevice()),
		  mSize(size)
	{
		for (uint32_t i = 0; i < VulkanBackend::MAX_FRAMES_IN_FLIGHT; i++)
		{
			VulkanUtils::CreateBuffer(size, vk::BufferUsageFlagBits::eStorageBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, mBuffer[i]);

			mMappedMemory[i] = mBuffer[i].Memory.mapMemory(0, size);

			mBufferInfo[i] = vk::DescriptorBufferInfo(mBuffer[i].Buffer, 0, mSize);
		}
	}

	VulkanStorageBuffer::~VulkanStorageBuffer()
	{
		for (size_t i = 0; i < VulkanBackend::MAX_FRAMES_IN_FLIGHT; i++)
		{
			mMappedMemory[i] = nullptr;
			mBuffer[i].Memory.unmapMemory();
		}
	}

	void VulkanStorageBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		if (!data)
			return;

		auto buffer_copy = CreateRef<std::vector<std::byte>>(size);
		std::memcpy(buffer_copy->data(), data, size);

		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();
		auto cmd = [=](const FVulkanFrameData &frame)
		{
			const auto current_frame = frame.Frame;

			std::memcpy(static_cast<std::byte *>(mMappedMemory[current_frame]) + offset, buffer_copy->data(), size);
			mBufferInfo[current_frame] = vk::DescriptorBufferInfo(mBuffer[current_frame].Buffer, 0, mSize);
		};

		api->SubmitCommand(cmd, ECommandType_PreCommand);
	}

	NativeHandle VulkanStorageBuffer::GetNativeHandle(uint32_t frame) const
	{
		return NativeHandle::FromPtr(&mBufferInfo[frame]);
	}

} // namespace BHive