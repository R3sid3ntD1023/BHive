#include "VulkanBuffer.h"
#include "VulkanBackend.h"
#include "VulkanMemory.h"
#include "VulkanUtils.h"

namespace BHive
{
	namespace utils
	{
		vk::BufferMemoryBarrier2 MakeBufferBarrier(vk::Buffer buffer, vk::PipelineStageFlags2 dstStage, vk::AccessFlags2 dstAccess)
		{
			return vk::BufferMemoryBarrier2(
				vk::PipelineStageFlagBits2::eCopy,
				vk::AccessFlagBits2::eTransferWrite,
				dstStage,
				dstAccess,
				VK_QUEUE_FAMILY_IGNORED,
				VK_QUEUE_FAMILY_IGNORED,
				buffer,
				0,
				VK_WHOLE_SIZE
			);
		}

	} // namespace utils

	VulkanBuffer::~VulkanBuffer()
	{
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
			mMappedPtrs[i] = nullptr;

		if (mLifeTime == EBufferLifetime::Static)
		{
			mBuffers[0].Destroy();
			mBuffers[1].Destroy();
		}
		else
		{
			for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
				mBuffers[i].Destroy();
		}
	}

	void VulkanBuffer::Initialize(size_t size, vk::BufferUsageFlags usage, EBufferLifetime lifeTime)
	{
		mLifeTime = lifeTime;
		(mLifeTime == EBufferLifetime::Static) ? InitStatic(size, usage) : InitDynamic(size, usage);
	}

	GPUBufferResourceHandle VulkanBuffer::GetNative(uint32_t frame) const
	{
		return (mLifeTime == EBufferLifetime::Static) ? mBuffers[0] : mBuffers[frame];
	}

	void VulkanBuffer::Upload(const void *data, size_t size, uint32_t offset)
	{
		if (mLifeTime != EBufferLifetime::Dynamic)
		{
			if (!data || size == 0)
				return;

			SingleTimeCommand cmd{};
			if (mMappedPtrs[1])
			{
				std::memcpy(static_cast<std::byte *>(mMappedPtrs[1]), data, size);
				vk::BufferCopy region(0, 0, size);
				cmd.Get().copyBuffer(mBuffers[1]->Buffer, mBuffers[0]->Buffer, region);
			}
			return;
		}

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			ASSERT(offset + size <= mBuffers[i]->Size);
			std::memcpy(static_cast<std::byte *>(mMappedPtrs[i]) + offset, data, size);
		}
	}

	void VulkanBuffer::ClearData()
	{
		if (mLifeTime != EBufferLifetime::Dynamic)
		{
			SingleTimeCommand cmd{};
			if (mMappedPtrs[1])
			{
				std::memset(static_cast<std::byte *>(mMappedPtrs[1]), 0, mBuffers[1]->Size);
				vk::BufferCopy region(0, 0, mBuffers[1]->Size);
				cmd.Get().copyBuffer(mBuffers[1]->Buffer, mBuffers[0]->Buffer, region);
			}
			return;
		}

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			std::memset(mMappedPtrs[i], 0, mBuffers[i]->Size);
		}
	}

	void VulkanBuffer::InitStatic(size_t size, vk::BufferUsageFlags usage)
	{
		auto info = vk::BufferCreateInfo({}, size, usage | vk::BufferUsageFlagBits::eTransferDst);
		mBuffers[0] = VulkanBackend::GetGPUResourceManager().CreateBuffer(info, vk::MemoryPropertyFlagBits::eDeviceLocal);

		auto stageInfo = vk::BufferCreateInfo({}, size, vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst);
		mBuffers[1] = VulkanBackend::GetGPUResourceManager().CreateBuffer(stageInfo, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

		mMappedPtrs[1] = mBuffers[1]->map(0, size);
	}

	void VulkanBuffer::InitDynamic(size_t size, vk::BufferUsageFlags usage)
	{
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			auto info = vk::BufferCreateInfo({}, size, usage);
			mBuffers[i] = VulkanBackend::GetGPUResourceManager().CreateBuffer(info, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
			mMappedPtrs[i] = mBuffers[i]->map(0, size);
		}
	}

} // namespace BHive