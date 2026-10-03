#include "VulkanBuffer.h"
#include "VulkanBackend.h"
#include "VulkanMemory.h"
#include "VulkanUtils.h"

namespace BHive
{
	VulkanBuffer::VulkanBuffer(vk::raii::Device &device)
		: mDevice(device)
	{
		mBuffers.reserve(MAX_FRAMES_IN_FLIGHT);
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
			mBuffers.emplace_back(nullptr);
	}

	VulkanBuffer::VulkanBuffer(VulkanBuffer &&other) noexcept
		: mDevice(other.mDevice),
		  mBuffers(std::move(other.mBuffers)),
		  mAllocations(other.mAllocations),
		  mSize(std::exchange(other.mSize, 0)),
		  mBufferCount(std::exchange(other.mBufferCount, 0)),
		  mLifeTime(other.mLifeTime)
	{
		other.mAllocations = {};
	}

	VulkanBuffer::~VulkanBuffer()
	{
		Reset();
	}

	void VulkanBuffer::Reset()
	{
		for (uint32_t i = 0; i < mBufferCount; i++)
		{
			mBuffers[i] = VK_NULL_HANDLE;
			VulkanBackend::GetMemoryAllocator().Free(mAllocations[i]);
			mAllocations[i] = {};
		}
		mBufferCount = 0;
		mSize = 0;
	}

	void VulkanBuffer::Initialize(size_t size, vk::BufferUsageFlags usage, EBufferLifetime lifeTime)
	{
		Reset();
		mSize = size;
		mLifeTime = lifeTime;
		(mLifeTime == EBufferLifetime::Static) ? InitStatic(size, usage) : InitDynamic(size, usage);
	}

	const vk::raii::Buffer &VulkanBuffer::GetNative(uint32_t frame) const
	{
		return mBuffers[(mLifeTime == EBufferLifetime::Static) ? 0 : frame];
	}

	void *VulkanBuffer::MapAllocation(uint32_t index)
	{
		auto &allocation = mAllocations[index];
		if (!allocation.IsMapped)
			allocation.MappedPtr = VulkanBackend::GetMemoryAllocator().Map(allocation);
		return allocation.MappedPtr;
	}

	void VulkanBuffer::CopyStaticBuffer(vk::DeviceSize size, vk::DeviceSize offset)
	{
		SingleTimeCommand cmd{};
		vk::BufferCopy region(offset, offset, size);
		cmd.Get().copyBuffer(*mBuffers[1], *mBuffers[0], region);
	}

	void VulkanBuffer::Upload(const void *data, size_t size, uint32_t offset)
	{
		if (mLifeTime != EBufferLifetime::Dynamic)
		{
			if (!data || size == 0)
				return;

			ASSERT(offset + size <= mSize);
			auto mapped = MapAllocation(1);
			std::memcpy(static_cast<std::byte *>(mapped) + offset, data, size);
			CopyStaticBuffer(size, offset);
			return;
		}

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			ASSERT(offset + size <= mSize);
			std::memcpy(static_cast<std::byte *>(MapAllocation(i)) + offset, data, size);
		}
	}

	void VulkanBuffer::ClearData()
	{
		if (mLifeTime != EBufferLifetime::Dynamic)
		{
			std::memset(MapAllocation(1), 0, mSize);
			CopyStaticBuffer(mSize);
			return;
		}

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			std::memset(MapAllocation(i), 0, mSize);
		}
	}

	void VulkanBuffer::InitStatic(size_t size, vk::BufferUsageFlags usage)
	{
		auto info = vk::BufferCreateInfo({}, size, usage | vk::BufferUsageFlagBits::eTransferDst);
		mBuffers[0] = mDevice.createBuffer(info);
		mAllocations[0] = VulkanBackend::GetMemoryAllocator().Allocate(mBuffers[0], vk::MemoryPropertyFlagBits::eDeviceLocal);
		mBuffers[0].bindMemory(mAllocations[0].Memory, mAllocations[0].Offset);

		auto stageInfo = vk::BufferCreateInfo({}, size, vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst);
		mBuffers[1] = mDevice.createBuffer(stageInfo);
		mAllocations[1] = VulkanBackend::GetMemoryAllocator().Allocate(mBuffers[1], vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
		mBuffers[1].bindMemory(mAllocations[1].Memory, mAllocations[1].Offset);
		mBufferCount = 2;
	}

	void VulkanBuffer::InitDynamic(size_t size, vk::BufferUsageFlags usage)
	{
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			auto info = vk::BufferCreateInfo({}, size, usage);
			mBuffers[i] = mDevice.createBuffer(info);
			mAllocations[i] = VulkanBackend::GetMemoryAllocator().Allocate(mBuffers[i], vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
			mBuffers[i].bindMemory(mAllocations[i].Memory, mAllocations[i].Offset);
		}
		mBufferCount = MAX_FRAMES_IN_FLIGHT;
	}

} // namespace BHive