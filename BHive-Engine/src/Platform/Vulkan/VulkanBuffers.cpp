#include "VulkanBuffers.h"
#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"
#include "VulkanBackend.h"

namespace BHive
{
	namespace utils
	{
		vk::BufferMemoryBarrier2 MakeBufferBarrier(vk::Buffer buffer, vk::PipelineStageFlagBits2 dstStage, vk::AccessFlags2 dstAccess)
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
		auto bufferID = VulkanBackend::GetGPUResourceManager().CreateBuffer(info, vk::MemoryPropertyFlagBits::eDeviceLocal, size);

		auto stageInfo = vk::BufferCreateInfo({}, size, vk::BufferUsageFlagBits::eTransferSrc);
		auto stageID = VulkanBackend::GetGPUResourceManager().CreateBuffer(stageInfo, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, size);

		Buffer = AllocatedBuffer{bufferID, size};
		StagingBuffer = AllocatedBuffer{stageID, size};
		VulkanBackend::GetGPUResourceManager().MapMemory(stageID, 0, size);
	}

	void VulkanStaticBuffer::Upload(vk::raii::CommandBuffer& cmd, const void *data, size_t size, uint32_t offset)
	{	
		auto mapped_memory = StagingBuffer.GetAllocation().MappedPtr;
		if (mapped_memory)
		{
			std::memcpy(static_cast<std::byte *>(mapped_memory) + offset, data, size);
			vk::BufferCopy region(0, offset, size);
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
			auto bufferID = VulkanBackend::GetGPUResourceManager().CreateBuffer(info, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, size);
			Buffers[i] = AllocatedBuffer{bufferID, size};
			VulkanBackend::GetGPUResourceManager().MapMemory(bufferID, 0, size);
		}
	}

	void VulkanPerFrameHostBuffer::Init(const void *data, size_t size, vk::BufferUsageFlags usage)
	{
		Init(size, usage);

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			Upload(i, data, size, 0);
		}
	}

	void VulkanPerFrameHostBuffer::Upload(uint32_t frame, const void *data, size_t size, uint32_t offset)
	{
		auto mapped = Buffers[frame].GetAllocation().MappedPtr;
		std::memcpy(static_cast<std::byte *>(mapped) + offset, data, size);
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

		auto buffer_copy = CreateRef<std::vector<std::byte>>(size);
		std::memcpy(buffer_copy->data(), data, size);

		RenderCommand::GetGraphicsAPI()->ExecuteTransferPass(
			[this, buffer_copy](ITransferContext &ctx)
			{
				auto &vk_ctx = ctx.As<FVulkanTransferContext>();
				auto &cmd = vk_ctx.Cmd;

				mBuffer.Upload(cmd, buffer_copy->data(), buffer_copy->size(), 0);

				auto barrier = utils::MakeBufferBarrier(mBuffer.Buffer.GetBuffer(), vk::PipelineStageFlagBits2::eIndexInput, vk::AccessFlagBits2::eIndexRead);
				vk::DependencyInfo depInfo({}, {}, barrier);
				cmd.pipelineBarrier2(depInfo);
			});
	}

	NativeHandle StaticVulkanIndexBuffer::GetNativeHandle(uint32_t frame) const
	{
		return NativeHandle::FromPtr(&mBuffer);
	}

	StaticVulkanVertexBuffer::StaticVulkanVertexBuffer(const void* data, size_t size)
	{
		ASSERT(data, "Data must be initilaized!");
		mBuffer.Init(size, vk::BufferUsageFlagBits::eVertexBuffer);

		auto buffer_copy = CreateRef<std::vector<std::byte>>(size);
		std::memcpy(buffer_copy->data(), data, size);

		RenderCommand::GetGraphicsAPI()->ExecuteTransferPass(
			[this, buffer_copy](ITransferContext &ctx)
			{
				auto &vk_ctx = ctx.As<FVulkanTransferContext>();
				auto &cmd = vk_ctx.Cmd;

				mBuffer.Upload(cmd, buffer_copy->data(), buffer_copy->size(), 0);

				auto barrier = utils::MakeBufferBarrier(mBuffer.Buffer.GetBuffer(), vk::PipelineStageFlagBits2::eVertexAttributeInput, vk::AccessFlagBits2::eVertexAttributeRead);
				vk::DependencyInfo depInfo({}, {}, barrier);
				cmd.pipelineBarrier2(depInfo);
			});
	}

	NativeHandle StaticVulkanVertexBuffer::GetNativeHandle(uint32_t frame) const
	{
		return NativeHandle::FromPtr(&mBuffer);
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

	void DynamicVulkanIndexBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		if (!data || size == 0)
			return;

		auto buffer_copy = CreateRef<std::vector<std::byte>>(size);
		std::memcpy(buffer_copy->data(), data, size);

		RenderCommand::SubmitResourceUpdate(
			[this, buffer_copy, offset](IRendererContext& ctx)
			{
				auto &vk_ctx = ctx.As<FVulkanRendererContext>();
				const auto frame = vk_ctx.Frame;
				mPerFrameBuffer.Upload(frame, buffer_copy->data(), buffer_copy->size(), offset);
			});
	}

	NativeHandle DynamicVulkanIndexBuffer::GetNativeHandle(uint32_t frame) const
	{
		ASSERT(frame < MAX_FRAMES_IN_FLIGHT)
		return NativeHandle::FromPtr(&mPerFrameBuffer.Buffers[frame]);
	}

	DynamicVulkanVertexBuffer::DynamicVulkanVertexBuffer(const void* data, const size_t size)
	{
		mPerFrameBuffer.Init(size, vk::BufferUsageFlagBits::eVertexBuffer);	
		if (data)
			SetData(data, size);
	}

	void DynamicVulkanVertexBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		if (!data || size == 0)
			return;

		auto buffer_copy = CreateRef<std::vector<std::byte>>(size);
		std::memcpy(buffer_copy->data(), data, size);

		RenderCommand::SubmitResourceUpdate(
			[this, buffer_copy, offset](IRendererContext &ctx)
			{
				auto &vk_ctx = ctx.As<FVulkanRendererContext>();
				const auto frame = vk_ctx.Frame;
				mPerFrameBuffer.Upload(frame, buffer_copy->data(), buffer_copy->size(), offset);
			});
	}

	void DynamicVulkanVertexBuffer::SetLayout(const BufferLayout &layout)
	{
		mLayout = layout;
	}

	NativeHandle DynamicVulkanVertexBuffer::GetNativeHandle(uint32_t frame) const
	{
		ASSERT(frame < MAX_FRAMES_IN_FLIGHT)
		return NativeHandle::FromPtr(&mPerFrameBuffer.Buffers[frame]);
	}

	VulkanGPUBuffer::VulkanGPUBuffer(size_t size, EBufferType type, const void *data)
		: mSize(size)
	{
		mPerFrameBuffer.Init(size, utils::ToVkBufferType(type));
		if (data)
			SetData(data, size, 0);
	}

	void VulkanGPUBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		if (!data)
			return;

		for (size_t frame = 0; frame < MAX_FRAMES_IN_FLIGHT; frame++)
		{
			mPerFrameBuffer.Upload(frame, data, size, offset);
		}	
	}

	NativeHandle VulkanGPUBuffer::GetNativeHandle(uint32_t frame) const
	{
		return Handle::Buffer(&mPerFrameBuffer.Buffers[frame]);
	}
} // namespace BHive