#include "VulkanUniformBuffer.h"
#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"
#include "VulkanBackend.h"

namespace BHive
{
	VulkanUniformBuffer::VulkanUniformBuffer(uint32_t binding, uint64_t size, const void *data)
		: mDevice(VulkanBackend::GetLogicalDevice()),
		  mSize(size)
	{
		
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			BufferDesc desc{};
			desc.Usage = vk::BufferUsageFlagBits::eUniformBuffer;
			desc.MemoryFlags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
			desc.Size = size;
			mBuffer[i] = VulkanBackend::GetGPUResourceManager().CreateBuffer(desc);

			VulkanBackend::GetGPUResourceManager().MapMemory(mBuffer[i], 0, size);
		}

		SetData(data, size, 0);	
	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		auto buffers = mBuffer;

		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			api->QueueDeletion(
				[buffers, i](uint32_t)
				{
					VulkanBackend::GetGPUResourceManager().DestroyBuffer(buffers[i]);
				});
		}
		
	}

	void VulkanUniformBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		if (!data)
			return;

		auto buffer_copy = CreateRef<std::vector<std::byte>>(size);
		std::memcpy(buffer_copy->data(), data, size);

		RenderCommand::SubmitResourceUpdate(
			[=](IRendererContext &ctx)
			{
				auto &vk_ctx = CastRef<FVulkanRendererContext>(ctx);

				const auto current_frame = vk_ctx.Frame;

				auto mapped_memory = mBuffer[current_frame].Allocation.MappedPtr;
				std::memcpy(static_cast<std::byte *>(mapped_memory) + offset, buffer_copy->data(), size);
				mBufferInfos[current_frame] = vk::DescriptorBufferInfo(mBuffer[current_frame].Buffer, 0, mSize);
			});
	}

	NativeHandle VulkanUniformBuffer::GetNativeHandle(uint32_t frame) const
	{
		ASSERT(frame < MAX_FRAMES_IN_FLIGHT);
		return Handle::BufferInfo(&mBufferInfos[frame]);
	}

} // namespace BHive