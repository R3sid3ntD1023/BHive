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
		VulkanUtils::CreateBuffer(size, vk::BufferUsageFlagBits::eStorageBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, mBuffer);

		SetData(data, size, 0);

		mMappedMemory = mBuffer.Memory.mapMemory(0, size);
	}

	VulkanStorageBuffer::VulkanStorageBuffer(size_t size)
		: mDevice(VulkanBackend::GetLogicalDevice()),
		  mSize(size)
	{
		VulkanUtils::CreateBuffer(size, vk::BufferUsageFlagBits::eStorageBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, mBuffer);
	}

	VulkanStorageBuffer::~VulkanStorageBuffer()
	{
		mMappedMemory = nullptr;
		mBuffer.Memory.unmapMemory();
	}

	void VulkanStorageBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		if (!data)
			return;

		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();
		auto cmd = [=](const FVulkanFrameData &frame)
		{
			const auto current_frame = frame.Frame;

			std::memcpy(static_cast<std::byte *>(mMappedMemory) + offset, data, size);
			mBufferInfo  = vk::DescriptorBufferInfo(mBuffer.Buffer, 0, mSize);
		};

		api->SubmitCommand(cmd, ECommandType_PreCommand);
	}

} // namespace BHive