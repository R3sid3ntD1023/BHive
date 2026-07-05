#include "VulkanBuffers.h"
#include "gfx/RenderCommand.h"
#include "gfx/renderers/Renderer.h"
#include "VulkanBackend.h"

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
				VK_WHOLE_SIZE);
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

	}

	void VulkanStaticBuffer::Init(size_t size, vk::BufferUsageFlags usage)
	{
		auto info = vk::BufferCreateInfo({}, size, usage | vk::BufferUsageFlagBits::eTransferDst);
		auto bufferID = VulkanBackend::GetGPUResourceManager().CreateBuffer(info, vk::MemoryPropertyFlagBits::eDeviceLocal);

		auto stageInfo = vk::BufferCreateInfo({}, size, vk::BufferUsageFlagBits::eTransferSrc);
		auto stageID = VulkanBackend::GetGPUResourceManager().CreateBuffer(stageInfo, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

		Buffer = AllocatedBuffer{bufferID, size};
		StagingBuffer = AllocatedBuffer{stageID, size};
		VulkanBackend::GetGPUResourceManager().MapMemory(stageID, 0, size);
	}

	void VulkanStaticBuffer::Upload(vk::CommandBuffer cmd, uint32_t frame, const FBufferUploadInfo &up)
	{	
		auto mapped_memory = StagingBuffer.GetAllocation().MappedPtr;
		if (mapped_memory)
		{
			std::memcpy(static_cast<std::byte *>(mapped_memory) + up.offset, up.data, up.size);
			vk::BufferCopy region(0, up.offset, up.size);
			cmd.copyBuffer(StagingBuffer.GetBuffer(), Buffer.GetBuffer(), region);
		}
	}

	VulkanStaticBuffer::~VulkanStaticBuffer()
	{
		VulkanBackend::GetGPUResourceManager().DestroyBuffer(Buffer);
		VulkanBackend::GetGPUResourceManager().DestroyBuffer(StagingBuffer);
	}

	void VulkanPerFrameHostBuffer::Init(size_t size, vk::BufferUsageFlags usage)
	{
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			auto info = vk::BufferCreateInfo({}, size, usage);
			auto bufferID = VulkanBackend::GetGPUResourceManager().CreateBuffer(info, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
			Buffers[i] = AllocatedBuffer{bufferID, size};
			VulkanBackend::GetGPUResourceManager().MapMemory(bufferID, 0, size);
		}
	}

	void VulkanPerFrameHostBuffer::Init(const void *data, size_t size, vk::BufferUsageFlags usage)
	{
		Init(size, usage);
	}

	void VulkanPerFrameHostBuffer::Upload(vk::CommandBuffer cmd, uint32_t frame, const FBufferUploadInfo &up)
	{
		auto mapped = Buffers[frame].GetAllocation().MappedPtr;
		std::memcpy(static_cast<std::byte *>(mapped) + up.offset, up.data, up.size);
	}

	VulkanPerFrameHostBuffer::~VulkanPerFrameHostBuffer()
	{
		for (auto &b : Buffers)
			VulkanBackend::GetGPUResourceManager().DestroyBuffer(b);
	}

	//-------------------Static Buffers---------------------------------------------//
	StaticVulkanIndexBuffer::StaticVulkanIndexBuffer(const uint32_t* data, uint32_t count)
		: mCount(count)
	{
		auto size = count * sizeof(uint32_t);
		mBuffer.Init(size, vk::BufferUsageFlagBits::eIndexBuffer);

		ASSERT(data, "Data must be initilaized!");

		RenderGraph init;

		auto &pass = init.AddPass("StaticIndexBufferUpload", EPassType::OffScreen);
		pass.BeginPhase(EPhaseType::Transfer);
		pass.Push(this, EBufferAccess::IndexRead);
		pass.Emplace<CmdUploadBuffer>()(this, data, size, 0);
		pass.EndPhase();

		Renderer::Get().ExecuteGraph(init);
	}

	StaticVulkanVertexBuffer::StaticVulkanVertexBuffer(const void* data, size_t size)
	{
		ASSERT(data, "Data must be initilaized!");
		mBuffer.Init(size, vk::BufferUsageFlagBits::eVertexBuffer);

		RenderGraph init;

		auto &pass = init.AddPass("StaticVertexBufferUpload", EPassType::OffScreen);
		pass.BeginPhase(EPhaseType::Transfer);
		pass.Push(this, EBufferAccess::VertexRead);
		pass.Emplace<CmdUploadBuffer>()(this, data, size, 0);
		pass.EndPhase();

		Renderer::Get().ExecuteGraph(init);
	}


	//------------------------Dynamic Buffers---------------------------------//
	DynamicVulkanIndexBuffer::DynamicVulkanIndexBuffer(const uint32_t *data, uint32_t count)
		: mCount(count)
	{
		auto size = count * sizeof(uint32_t);
		mPerFrameBuffer.Init(size, vk::BufferUsageFlagBits::eIndexBuffer);
		if (data)
			SetData(data, size, 0);
	}

	DynamicVulkanVertexBuffer::DynamicVulkanVertexBuffer(const void* data, const size_t size)
	{
		mPerFrameBuffer.Init(size, vk::BufferUsageFlagBits::eVertexBuffer);	
		if (data)
			SetData(data, size);
	}

	void DynamicVulkanVertexBuffer::SetLayout(const BufferLayout &layout)
	{
		mLayout = layout;
	}

	VulkanGPUBuffer::VulkanGPUBuffer(size_t size, EBufferType type, const void *data)
		: mSize(size)
	{
		mPerFrameBuffer.Init(size, utils::ToVkBufferType(type));
		if (data)
			SetData(data, size, 0);
	}

} // namespace BHive