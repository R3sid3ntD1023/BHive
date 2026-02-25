#include "VulkanBuffers.h"
#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"
#include "GPUResourceManager.h"

namespace BHive
{
	void PerFrameBuffer::Init(size_t size, vk::BufferUsageFlags usage)
	{
		BufferDesc desc{};
		desc.MemoryFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
		desc.Size = size;
		desc.Usage = usage | vk::BufferUsageFlagBits::eTransferDst;
		Buffer =  GPUResourceManager::Get().CreateBuffer(desc);


		desc.MemoryFlags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
		desc.Size = size;
		desc.Usage = vk::BufferUsageFlagBits::eTransferSrc;
		StagingBuffer = GPUResourceManager::Get().CreateBuffer(desc);

		MappedMemory = GPUResourceManager::Get().MapMemory(StagingBuffer,0, size);
	}

	void PerFrameBuffer::SetData(vk::raii::CommandBuffer &cmd, const void *data, size_t size, uint32_t offset, vk::PipelineStageFlags2 flags, vk::AccessFlags2 access)
	{
		if (MappedMemory)
		{
			std::memcpy(static_cast<std::byte *>(MappedMemory) + offset, data, size);
		}

		vk::BufferCopy copy_region(0, offset, size);
		cmd.copyBuffer(StagingBuffer.Buffer, Buffer.Buffer, copy_region);

		vk::BufferMemoryBarrier2 barrier(
			vk::PipelineStageFlagBits2::eCopy, vk::AccessFlagBits2::eTransferWrite, flags, access, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, Buffer.Buffer, 0, size);

		vk::DependencyInfo dependency_info({}, {}, barrier);

		cmd.pipelineBarrier2(dependency_info);
	}


	NativeHandle PerFrameBuffer::GetNativeHandle() const
	{
		return NativeHandle::FromPtr(&Buffer.Buffer);
	}

	PerFrameBuffer::~PerFrameBuffer()
	{
		auto buffer = Buffer;
		auto stagingBuffer = StagingBuffer;

		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();
		api->QueueDeletion([buffer, stagingBuffer](uint32_t) { 
				auto& gpu_r_m = GPUResourceManager::Get();
				gpu_r_m.DestroyBuffer(buffer);
				gpu_r_m.DestroyBuffer(stagingBuffer);
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

		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();
		auto cmd = [this, buffer_copy, size, offset](const FVulkanFrame &frame)
		{ mBuffer.SetData(frame.CommandBuffer, buffer_copy->data(), size, offset, vk::PipelineStageFlagBits2::eIndexInput, vk::AccessFlagBits2::eIndexRead); };

		api->SubmitCommand(cmd, ECommandType_PreCommand);
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

		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();
		auto cmd = [this, buffer_copy, size, offset](const FVulkanFrame &frame)
		{ mBuffer.SetData(frame.CommandBuffer, buffer_copy->data(), size, offset, vk::PipelineStageFlagBits2::eVertexAttributeInput, vk::AccessFlagBits2::eVertexAttributeRead); };

		api->SubmitCommand(cmd, ECommandType_PreCommand);
	}

	//------------------------Dynamic Buffers---------------------------------//
	DynamicVulkanIndexBuffer::DynamicVulkanIndexBuffer(uint32_t count)
		: mDevice(VulkanBackend::GetLogicalDevice()),
		  mCount(count)
	{
		for (uint32_t i = 0; i < VulkanBackend::MAX_FRAMES_IN_FLIGHT; i++)
		{
			mPerFrameBuffer[i].Init(count * sizeof(uint32_t), vk::BufferUsageFlagBits::eIndexBuffer);
		}
	}

	void DynamicVulkanIndexBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		if (!data || size == 0)
			return;

		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();

		auto cmd = [this, data, size, offset](const FVulkanFrame &frame)
		{ 
			mPerFrameBuffer[frame.Frame].SetData(frame.CommandBuffer, data, size, offset, vk::PipelineStageFlagBits2::eIndexInput, vk::AccessFlagBits2::eIndexRead);
		};

		api->SubmitCommand(cmd, ECommandType_PreCommand);
	}

	DynamicVulkanVertexBuffer::DynamicVulkanVertexBuffer(const size_t size)
		: mDevice(VulkanBackend::GetLogicalDevice())
	{
		for (uint32_t i = 0; i < VulkanBackend::MAX_FRAMES_IN_FLIGHT; i++)
		{
			mPerFrameBuffer[i].Init(size, vk::BufferUsageFlagBits::eVertexBuffer);	
		}
	}

	void DynamicVulkanVertexBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		if (!data || size == 0)
			return;

		auto api = RenderCommand::GetRendererAPI<VulkanRendererAPI>();

		auto cmd = [this, data, size, offset](const FVulkanFrame &frame)
		{ 
			mPerFrameBuffer[frame.Frame].SetData(frame.CommandBuffer, data, size, offset, vk::PipelineStageFlagBits2::eVertexAttributeInput, vk::AccessFlagBits2::eVertexAttributeRead);
		};

		api->SubmitCommand(cmd, ECommandType_PreCommand);
	}

	void DynamicVulkanVertexBuffer::SetLayout(const BufferLayout &layout)
	{
		mLayout = layout;
	}

	

} // namespace BHive