#include "VulkanBuffers.h"
#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"
#include "VulkanBackend.h"
#include "VulkanConverters.h"

namespace BHive
{
	void PerFrameBuffer::Init(size_t size, vk::BufferUsageFlags usage)
	{
		BufferDesc desc{};
		desc.MemoryFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
		desc.Size = size;
		desc.Usage = usage | vk::BufferUsageFlagBits::eTransferDst;
		Buffer = VulkanBackend::GetGPUResourceManager().CreateBuffer(desc);


		desc.MemoryFlags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
		desc.Size = size;
		desc.Usage = vk::BufferUsageFlagBits::eTransferSrc;
		StagingBuffer = VulkanBackend::GetGPUResourceManager().CreateBuffer(desc);

		VulkanBackend::GetGPUResourceManager().MapMemory(StagingBuffer, 0, size);

	}

	void PerFrameBuffer::SetData(vk::raii::CommandBuffer &cmd, const void *data, size_t size, uint32_t offset, vk::PipelineStageFlags2 flags, vk::AccessFlags2 access)
	{	
		auto mapped_memory = StagingBuffer.Allocation.MappedPtr;
		if (mapped_memory)
			std::memcpy(static_cast<std::byte *>(mapped_memory) + offset, data, size);

		vk::BufferCopy copy_region(0, offset, size);
		cmd.copyBuffer(StagingBuffer.Buffer, Buffer.Buffer, copy_region);

		vk::BufferMemoryBarrier2 barrier(
			vk::PipelineStageFlagBits2::eCopy, vk::AccessFlagBits2::eTransferWrite, flags, access, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, Buffer.Buffer, 0, size);

		vk::DependencyInfo dependency_info({}, {}, barrier);

		cmd.pipelineBarrier2(dependency_info);
	}


	NativeHandle PerFrameBuffer::GetNativeHandle() const
	{
		return NativeHandle::FromPtr(&Buffer);
	}

	PerFrameBuffer::~PerFrameBuffer()
	{
		auto buffer = Buffer;
		auto stagingBuffer = StagingBuffer;

		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();
		api->QueueDeletion([buffer, stagingBuffer](uint32_t) { 
				VulkanBackend::GetGPUResourceManager().DestroyBuffer(buffer);
				VulkanBackend::GetGPUResourceManager().DestroyBuffer(stagingBuffer);
			});
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

		RenderCommand::SubmitResourceUpdate(
			[this, buffer_copy, size, offset](IRendererContext &ctx)
			{
				auto &vk_ctx = CastRef<FVulkanRendererContext>(ctx);
				mBuffer.SetData(vk_ctx.CommandBuffer, buffer_copy->data(), size, offset, vk::PipelineStageFlagBits2::eIndexInput, vk::AccessFlagBits2::eIndexRead);
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

		RenderCommand::SubmitResourceUpdate(
			[this, buffer_copy, size, offset](IRendererContext &ctx)
			{
				auto &vk_ctx = CastRef<FVulkanRendererContext>(ctx);
				mBuffer.SetData(vk_ctx.CommandBuffer, buffer_copy->data(), size, offset, vk::PipelineStageFlagBits2::eVertexAttributeInput, vk::AccessFlagBits2::eVertexAttributeRead);
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
			[this, data, size, offset](IRendererContext &ctx)
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

	VulkanGPUBuffer::VulkanGPUBuffer(uint32_t binding, size_t size, EBufferType type, const void *data)
		: mDevice(VulkanBackend::GetLogicalDevice()),
		  mBinding(binding),
		  mSize(size)
	{
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			BufferDesc desc{};
			desc.Usage = ToVkBufferType(type);
			desc.MemoryFlags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
			desc.Size = size;
			mBuffer[i] = VulkanBackend::GetGPUResourceManager().CreateBuffer(desc);

			VulkanBackend::GetGPUResourceManager().MapMemory(mBuffer[i], 0, size);

			SetData(data, size, 0);
		}
	}

	VulkanGPUBuffer::VulkanGPUBuffer(uint32_t binding, size_t size, EBufferType type)
		: mDevice(VulkanBackend::GetLogicalDevice()),
		  mBinding(binding),
		  mSize(size)
	{
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			BufferDesc desc{};
			desc.Usage = ToVkBufferType(type);
			desc.MemoryFlags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
			desc.Size = size;
			mBuffer[i] = VulkanBackend::GetGPUResourceManager().CreateBuffer(desc);

			VulkanBackend::GetGPUResourceManager().MapMemory(mBuffer[i], 0, size);
		}
	}

	VulkanGPUBuffer::~VulkanGPUBuffer()
	{
		auto buffers = mBuffer;

		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			api->QueueDeletion([buffers, i](uint32_t) { VulkanBackend::GetGPUResourceManager().DestroyBuffer(buffers[i]); });
		}
	}

	void VulkanGPUBuffer::SetData(const void *data, size_t size, uint32_t offset)
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
			});
	}

	NativeHandle VulkanGPUBuffer::GetNativeHandle(uint32_t frame) const
	{
		return Handle::Buffer(&mBuffer[frame]);
	}

} // namespace BHive