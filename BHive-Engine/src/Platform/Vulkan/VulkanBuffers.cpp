#include "VulkanBuffers.h"
#include "gfx/RenderCommand.h"
#include "gfx/renderers/Renderer.h"
#include "VulkanBackend.h"
#include "VulkanUtils.h"

namespace BHive
{
	namespace utils
	{
		vk::BufferMemoryBarrier2 MakeBufferBarrier(vk::Buffer buffer, vk::PipelineStageFlags2 dstStage, vk::AccessFlags2 dstAccess)
		{
			return vk::BufferMemoryBarrier2(
				vk::PipelineStageFlagBits2::eCopy, vk::AccessFlagBits2::eTransferWrite, dstStage, dstAccess, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, buffer, 0, VK_WHOLE_SIZE);
		}

		vk::BufferUsageFlags ToVkBufferType(EBufferType type)
		{
			switch (type)
			{
			case BHive::EBufferType::UniformBuffer:
				return vk::BufferUsageFlagBits::eUniformBuffer;
			case BHive::EBufferType::StorageBuffer:
				return vk::BufferUsageFlagBits::eStorageBuffer;
			case BHive::EBufferType::IndirectBuffer:
				return vk::BufferUsageFlagBits::eIndirectBuffer;
			default:
				break;
			}
			ASSERT(false)
			return (vk::BufferUsageFlagBits)0;
		}

	} // namespace utils

	VulkanBuffer::~VulkanBuffer()
	{
		auto &mng = VulkanBackend::GetGPUResourceManager();
		if (mLifeTime == EBufferLifetime::Static)
			mng.DestroyBuffer(mBuffers[0]);
		else
		{
			for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
				mng.DestroyBuffer(mBuffers[i]);
		}
	}

	void VulkanBuffer::Init(size_t size, const void *data, vk::BufferUsageFlags usage, EBufferLifetime lifeTime)
	{
		mLifeTime = lifeTime;
		(mLifeTime == EBufferLifetime::Static) ? InitStatic(size, data, usage) : InitDynamic(size, data, usage);
	}

	const AllocatedBuffer &VulkanBuffer::GetNative(uint32_t frame) const
	{
		return (mLifeTime == EBufferLifetime::Static) ? mBuffers[0] : mBuffers[frame];
	}

	void VulkanBuffer::Upload(const FBufferUploadInfo &up)
	{
		if (mLifeTime != EBufferLifetime::Dynamic)
			return;

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			auto mapped = mBuffers[i].GetAllocation().MappedPtr;
			std::memcpy(static_cast<std::byte *>(mapped) + up.offset, up.data, up.size);
		}
	}

	void VulkanBuffer::InitStatic(size_t size, const void *data, vk::BufferUsageFlags usage)
	{
		auto info = vk::BufferCreateInfo({}, size, usage | vk::BufferUsageFlagBits::eTransferDst);
		auto bufferID = VulkanBackend::GetGPUResourceManager().CreateBuffer(info, vk::MemoryPropertyFlagBits::eDeviceLocal);

		auto stageInfo = vk::BufferCreateInfo({}, size, vk::BufferUsageFlagBits::eTransferSrc);
		auto stageID = VulkanBackend::GetGPUResourceManager().CreateBuffer(stageInfo, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

		mBuffers[0] = AllocatedBuffer{bufferID, size};
		mBuffers[1] = AllocatedBuffer{stageID, size};
		VulkanBackend::GetGPUResourceManager().MapMemory(stageID, 0, size);

		SingleTimeCommand cmd{};
		auto mapped_memory = mBuffers[1].GetAllocation().MappedPtr;
		if (mapped_memory)
		{
			std::memcpy(static_cast<std::byte *>(mapped_memory), data, size);
			vk::BufferCopy region(0, 0, size);
			cmd.Get().copyBuffer(mBuffers[1].GetBuffer(), mBuffers[0].GetBuffer(), region);
		}
	}

	void VulkanBuffer::InitDynamic(size_t size, const void *data, vk::BufferUsageFlags usage)
	{
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			auto info = vk::BufferCreateInfo({}, size, usage);
			auto bufferID = VulkanBackend::GetGPUResourceManager().CreateBuffer(info, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
			mBuffers[i] = AllocatedBuffer{bufferID, size};
			VulkanBackend::GetGPUResourceManager().MapMemory(bufferID, 0, size);
		}

		if (data)
		{
			FBufferUploadInfo info{.size = size, .offset = 0, .data = data};
			Upload(info);
		}
	}

	VulkanIndexBuffer::VulkanIndexBuffer(uint32_t count, EBufferLifetime lifeTime, const uint32_t *data)
		: mCount(count)
	{
		mBuffer.Init(count * sizeof(uint32_t), data, vk::BufferUsageFlagBits::eIndexBuffer, lifeTime);
	}

	VulkanVertexBuffer::VulkanVertexBuffer(size_t size, EBufferLifetime lifeTime, const void *data)
	{
		mBuffer.Init(size, data, vk::BufferUsageFlagBits::eVertexBuffer, lifeTime);
	}

	VulkanGeneralBuffer::VulkanGeneralBuffer(size_t size, EBufferType type, EBufferLifetime lifeTime, const void *data)
	{
		mBuffer.Init(size, data, utils::ToVkBufferType(type), lifeTime);
	}
} // namespace BHive