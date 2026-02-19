#include "VulkanUniformBuffer.h"
#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"
#include "GPUResourceManager.h"

namespace BHive
{
	VulkanUniformBuffer::VulkanUniformBuffer(uint32_t binding, uint64_t size, const void *data)
		: mDevice(VulkanBackend::GetLogicalDevice()),
		  mSize(size)
	{
		
		for (size_t i = 0; i < VulkanBackend::MAX_FRAMES_IN_FLIGHT; i++)
		{
			BufferDesc desc{};
			desc.Usage = vk::BufferUsageFlagBits::eUniformBuffer;
			desc.MemoryFlags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
			desc.Size = size;
			mBuffer[i] = GPUResourceManager::Get().CreateBuffer(desc);

			mMappedMemory[i] = GPUResourceManager::Get().MapMemory(mBuffer[i], 0, size);
		}

		SetData(data, size, 0);	
	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		
	}

	void VulkanUniformBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		if (!data)
			return;

		auto buffer_copy = CreateRef<std::vector<std::byte>>(size);
		std::memcpy(buffer_copy->data(), data, size);

		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();
		auto cmd = [=](const FVulkanFrame& frame)
			{		
				ASSERT(offset + size <= mSize);

				const auto current_frame = frame.Frame;
				std::memcpy(static_cast<std::byte *>(mMappedMemory[current_frame]) + offset, buffer_copy->data(), size);
				mBufferInfos[current_frame] = vk::DescriptorBufferInfo(mBuffer[current_frame].Buffer, 0, mSize);
			};
		
		api->SubmitCommand(cmd, ECommandType_PreCommand);
	}

	NativeHandle VulkanUniformBuffer::GetNativeHandle(uint32_t frame) const
	{
		ASSERT(frame < VulkanBackend::MAX_FRAMES_IN_FLIGHT);
		return Vulkan::Handle::BufferInfo(&mBufferInfos[frame]);
	}

} // namespace BHive