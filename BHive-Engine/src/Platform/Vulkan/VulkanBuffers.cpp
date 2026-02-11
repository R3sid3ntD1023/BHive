#include "VulkanBuffers.h"
#include "VulkanUtils.h"
#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"

namespace BHive
{
	void PerFrameBuffer::Init(size_t size, vk::BufferUsageFlags usage)
	{
		VulkanUtils::CreateBuffer(size, usage | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal, Buffer);
		VulkanUtils::CreateBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, StagingBuffer);
		MappedMemory = StagingBuffer.Memory.mapMemory(0, size);
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

	void PerFrameBuffer::Release()
	{
		LOG_TRACE("PerFrameBuffer Release Called")

		MappedMemory = nullptr;
		StagingBuffer.Memory.unmapMemory();
	}

	PerFrameBuffer::~PerFrameBuffer()
	{
		LOG_TRACE("PerFrameBuffer Destructor Called");
	}

	//-------------------Static Buffers---------------------------------------------//
	StaticVulkanIndexBuffer::StaticVulkanIndexBuffer(uint32_t count)
		: mDevice(VulkanCore::GetLogicalDevice()),
		  mCount(count)
	{
		mBuffer.Init(count * sizeof(uint32_t), vk::BufferUsageFlagBits::eIndexBuffer);
		LOG_TRACE("Created Static Index Buffer");
	}

	StaticVulkanIndexBuffer::~StaticVulkanIndexBuffer()
	{

		mBuffer.Release();

	}

	void StaticVulkanIndexBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		if (!data || size == 0)
			return;

		auto buffer_copy = CreateRef<std::vector<std::byte>>(size);
		std::memcpy(buffer_copy->data(), data, size);

		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();
		auto cmd = [this, buffer_copy, size, offset](const FVulkanFrameData &frame)
		{ mBuffer.SetData(frame.CommandBuffer, buffer_copy->data(), size, offset, vk::PipelineStageFlagBits2::eIndexInput, vk::AccessFlagBits2::eIndexRead); };

		api->SubmitCommand(cmd, ECommandType_PreCommand);
	}

	StaticVulkanVertexBuffer::StaticVulkanVertexBuffer(size_t size)
		: mDevice(VulkanCore::GetLogicalDevice())
	{
		mBuffer.Init(size, vk::BufferUsageFlagBits::eVertexBuffer);
		LOG_TRACE("Created Static Vertex Buffer")
	}

	StaticVulkanVertexBuffer::~StaticVulkanVertexBuffer()
	{
		mBuffer.Release();
	}

	void StaticVulkanVertexBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		if (!data || size == 0)
			return;

		auto buffer_copy = CreateRef<std::vector<std::byte>>(size);
		std::memcpy(buffer_copy->data(), data, size);

		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();
		auto cmd = [this, buffer_copy, size, offset](const FVulkanFrameData &frame)
		{ mBuffer.SetData(frame.CommandBuffer, buffer_copy->data(), size, offset, vk::PipelineStageFlagBits2::eVertexAttributeInput, vk::AccessFlagBits2::eVertexAttributeRead); };

		api->SubmitCommand(cmd, ECommandType_PreCommand);
	}

	//------------------------Dynamic Buffers---------------------------------//
	DynamicVulkanIndexBuffer::DynamicVulkanIndexBuffer(uint32_t count)
		: mDevice(VulkanCore::GetLogicalDevice()),
		  mCount(count)
	{
		for (uint32_t i = 0; i < VulkanCore::MAX_FRAMES_IN_FLIGHT; i++)
		{
			mPerFrameBuffer[i].Init(count * sizeof(uint32_t), vk::BufferUsageFlagBits::eIndexBuffer);
		}
	}

	DynamicVulkanIndexBuffer::~DynamicVulkanIndexBuffer()
	{
		for (uint32_t i = 0; i < VulkanCore::MAX_FRAMES_IN_FLIGHT; i++)
		{
			mPerFrameBuffer[i].Release();
		}
	}

	void DynamicVulkanIndexBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		if (!data || size == 0)
			return;

		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();

		auto cmd = [this, data, size, offset](const FVulkanFrameData &frame)
		{ 
			mPerFrameBuffer[frame.Frame].SetData(frame.CommandBuffer, data, size, offset, vk::PipelineStageFlagBits2::eIndexInput, vk::AccessFlagBits2::eIndexRead);
		};

		api->SubmitCommand(cmd, ECommandType_PreCommand);
	}

	DynamicVulkanVertexBuffer::DynamicVulkanVertexBuffer(const size_t size)
		: mDevice(VulkanCore::GetLogicalDevice())
	{
		for (uint32_t i = 0; i < VulkanCore::MAX_FRAMES_IN_FLIGHT; i++)
		{
			mPerFrameBuffer[i].Init(size, vk::BufferUsageFlagBits::eVertexBuffer);	
		}
	}

	DynamicVulkanVertexBuffer::~DynamicVulkanVertexBuffer()
	{
		for (uint32_t i = 0; i < VulkanCore::MAX_FRAMES_IN_FLIGHT; i++)
		{
			mPerFrameBuffer[i].Release();
		}
	}

	void DynamicVulkanVertexBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		if (!data || size == 0)
			return;

		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();

		auto cmd = [this, data, size, offset](const FVulkanFrameData &frame)
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