#include "VulkanBuffers.h"
#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"
#include "VulkanBackend.h"
#include "VulkanConverters.h"

namespace BHive
{
	void PerFrameBuffer::Init(size_t size, vk::BufferUsageFlags usage)
	{
		auto info = vk::BufferCreateInfo({}, size, usage | vk::BufferUsageFlagBits::eTransferDst);
		auto bufferID = VulkanBackend::GetGPUResourceManager().CreateBuffer(info, vk::MemoryPropertyFlagBits::eDeviceLocal, size);

		auto stagingInfo = vk::BufferCreateInfo({}, size, vk::BufferUsageFlagBits::eTransferSrc);
		auto stagingID = VulkanBackend::GetGPUResourceManager().CreateBuffer(stagingInfo, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, size);

		Buffer = AllocatedBuffer{.Buffer = bufferID, .Size = size};
		StagingBuffer = AllocatedBuffer{.Buffer = stagingID, .Size = size};
		VulkanBackend::GetGPUResourceManager().MapMemory(stagingID, 0, size);

	}

	void PerFrameBuffer::SetData(vk::raii::CommandBuffer& cmd, const void *data, size_t size, uint32_t offset, vk::PipelineStageFlags2 flags, vk::AccessFlags2 access)
	{	
		auto mapped_memory = StagingBuffer.GetAllocation().MappedPtr;
		if (mapped_memory)
			std::memcpy(static_cast<std::byte *>(mapped_memory) + offset, data, size);

		auto& src_buffer = StagingBuffer.GetBuffer();
		auto &dst_buffer = Buffer.GetBuffer();

		vk::BufferCopy copy_region(0, offset, size);
		cmd.copyBuffer(src_buffer, dst_buffer, copy_region);

		vk::BufferMemoryBarrier2 barrier(
			vk::PipelineStageFlagBits2::eCopy, vk::AccessFlagBits2::eTransferWrite, flags, access, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, dst_buffer, 0, size);

		vk::DependencyInfo dependency_info({}, {}, barrier);

		cmd.pipelineBarrier2(dependency_info);
	}


	NativeHandle PerFrameBuffer::GetNativeHandle() const
	{
		return NativeHandle::FromPtr(&Buffer);
	}

	PerFrameBuffer::~PerFrameBuffer()
	{
		VulkanBackend::GetGPUResourceManager().DestroyBuffer(Buffer);
		VulkanBackend::GetGPUResourceManager().DestroyBuffer(StagingBuffer);
	}

	//-------------------Static Buffers---------------------------------------------//
	StaticVulkanIndexBuffer::StaticVulkanIndexBuffer(uint32_t count)
		: mDevice(VulkanBackend::GetLogicalDevice()),
		  mCount(count)
	{
		mBuffer.Init(count * sizeof(uint32_t), vk::BufferUsageFlagBits::eIndexBuffer);
	}

	void StaticVulkanIndexBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		if (!data || size == 0)
			return;

		auto buffer_copy = CreateRef<std::vector<std::byte>>(size);
		std::memcpy(buffer_copy->data(), data, size);

		RenderCommand::GetGraphicsAPI()->ExecuteTransferPass(
			[this, buffer_copy, size, offset](ITransferContext& ctx)
			{
				auto &vk_ctx = CastRef<FVulkanTransferContext>(ctx);
				mBuffer.SetData(vk_ctx.Cmd, buffer_copy->data(), size, offset, vk::PipelineStageFlagBits2::eIndexInput, vk::AccessFlagBits2::eIndexRead);
			});
	}

	StaticVulkanVertexBuffer::StaticVulkanVertexBuffer(size_t size)
		: mDevice(VulkanBackend::GetLogicalDevice())
	{
		mBuffer.Init(size, vk::BufferUsageFlagBits::eVertexBuffer);
	}

	void StaticVulkanVertexBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		if (!data || size == 0)
			return;

		auto buffer_copy = CreateRef<std::vector<std::byte>>(size);
		std::memcpy(buffer_copy->data(), data, size);

		RenderCommand::GetGraphicsAPI()->ExecuteTransferPass(
			[this, buffer_copy, size, offset](ITransferContext &ctx)
			{
				auto &vk_ctx = CastRef<FVulkanTransferContext>(ctx);
				mBuffer.SetData(vk_ctx.Cmd, buffer_copy->data(), size, offset, vk::PipelineStageFlagBits2::eVertexAttributeInput, vk::AccessFlagBits2::eVertexAttributeRead);
			});
	}

	//------------------------Dynamic Buffers---------------------------------//
	DynamicVulkanIndexBuffer::DynamicVulkanIndexBuffer(uint32_t count)
		: mDevice(VulkanBackend::GetLogicalDevice()),
		  mCount(count)
	{
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			mPerFrameBuffer[i].Init(count * sizeof(uint32_t), vk::BufferUsageFlagBits::eIndexBuffer);
		}
	}

	void DynamicVulkanIndexBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		if (!data || size == 0)
			return;

		RenderCommand::SubmitResourceUpdate(
			[this, data, size, offset](IRendererContext& ctx)
			{
				auto &vk_ctx = CastRef<FVulkanRendererContext>(ctx);
				mPerFrameBuffer[vk_ctx.Frame].SetData(vk_ctx.CommandBuffer, data, size, offset, vk::PipelineStageFlagBits2::eIndexInput, vk::AccessFlagBits2::eIndexRead);
			});
	}

	DynamicVulkanVertexBuffer::DynamicVulkanVertexBuffer(const size_t size)
		: mDevice(VulkanBackend::GetLogicalDevice())
	{
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			mPerFrameBuffer[i].Init(size, vk::BufferUsageFlagBits::eVertexBuffer);	
		}
	}

	void DynamicVulkanVertexBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		if (!data || size == 0)
			return;

		RenderCommand::SubmitResourceUpdate(
			[this, data, size, offset](IRendererContext &ctx)
			{
				auto &vk_ctx = CastRef<FVulkanRendererContext>(ctx);

				mPerFrameBuffer[vk_ctx.Frame].SetData(vk_ctx.CommandBuffer, data, size, offset, vk::PipelineStageFlagBits2::eVertexAttributeInput, vk::AccessFlagBits2::eVertexAttributeRead);
			});
	}

	void DynamicVulkanVertexBuffer::SetLayout(const BufferLayout &layout)
	{
		mLayout = layout;
	}

	VulkanGPUBuffer::VulkanGPUBuffer(size_t size, EBufferType type, const void *data)
		: mDevice(VulkanBackend::GetLogicalDevice()),
		  mSize(size)
	{
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			auto info = vk::BufferCreateInfo({}, size, ToVkBufferType(type));
			auto bufferID = VulkanBackend::GetGPUResourceManager().CreateBuffer(info, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, size);

			mBuffer[i] = AllocatedBuffer{.Buffer = bufferID, .Size = size}; 
			VulkanBackend::GetGPUResourceManager().MapMemory(bufferID, 0, size);

			SetData(data, size, 0);
		}
	}

	VulkanGPUBuffer::VulkanGPUBuffer(size_t size, EBufferType type)
		: mDevice(VulkanBackend::GetLogicalDevice()),
		  mSize(size)
	{
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			auto info = vk::BufferCreateInfo({}, size, ToVkBufferType(type));
			auto bufferID = VulkanBackend::GetGPUResourceManager().CreateBuffer(info, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, size);

			mBuffer[i] = AllocatedBuffer{.Buffer = bufferID, .Size = size};
			VulkanBackend::GetGPUResourceManager().MapMemory(bufferID, 0, size);
		}
	}

	VulkanGPUBuffer::~VulkanGPUBuffer()
	{
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			VulkanBackend::GetGPUResourceManager().DestroyBuffer(mBuffer[i]);
		}
	}

	void VulkanGPUBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		if (!data)
			return;

		for (size_t frame = 0; frame < MAX_FRAMES_IN_FLIGHT; frame++)
		{
			auto mapped_memory = mBuffer[frame].GetAllocation().MappedPtr;
			std::memcpy(static_cast<std::byte *>(mapped_memory) + offset, data, size);
		}	
	}

	NativeHandle VulkanGPUBuffer::GetNativeHandle(uint32_t frame) const
	{
		return Handle::Buffer(&mBuffer[frame]);
	}

} // namespace BHive