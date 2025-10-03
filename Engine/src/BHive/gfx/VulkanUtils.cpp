#include "VulkanUtils.h"
#include "GraphicsContext.h"

namespace BHive
{

	vk::raii::CommandBuffer VulkanUtils::BeginSingleTimeCommands()
	{
		auto &cmdPool = GraphicsContext::Get().GetCommandPool();
		auto &device = GraphicsContext::Get().GetDevice();

		vk::CommandBufferAllocateInfo allocInfo(cmdPool, vk::CommandBufferLevel::ePrimary, 1);
		vk::raii::CommandBuffer commandBuffer = std::move(device.allocateCommandBuffers(allocInfo).front());
		commandBuffer.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
		return commandBuffer;
	}

	void VulkanUtils::EndSingleTimeCommands(vk::raii::CommandBuffer &commandBuffer)
	{
		commandBuffer.end();

		vk::SubmitInfo submitInfo({}, {}, *commandBuffer);

		auto graphics_queue = GraphicsContext::Get().GetGraphicsQueue();
		graphics_queue.submit(submitInfo, nullptr);
		graphics_queue.waitIdle();
	}

	void VulkanUtils::CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Buffer &buffer, vk::raii::DeviceMemory &bufferMemory)
	{
		auto &device = GraphicsContext::Get().GetDevice();
		vk::BufferCreateInfo bufferCreateInfo({}, size, usage, vk::SharingMode::eExclusive);
		buffer = device.createBuffer(bufferCreateInfo);

		vk::MemoryRequirements memRequirements = buffer.getMemoryRequirements();
		vk::MemoryAllocateInfo allocInfo(memRequirements.size, FindMemoryType(memRequirements.memoryTypeBits, properties));
		bufferMemory = device.allocateMemory(allocInfo);
		buffer.bindMemory(*bufferMemory, 0);
	}

	void VulkanUtils::CreateImage2D(
		uint32_t w, uint32_t h, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Image &image, vk::raii::DeviceMemory &imageMemory)
	{
		auto &device = GraphicsContext::Get().GetDevice();
		vk::ImageCreateInfo imageInfo(
			{}, vk::ImageType::e2D, format, {w, h, 1}, 1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
			vk::SharingMode::eExclusive, 0);
		image = device.createImage(imageInfo);

		vk::MemoryRequirements memRequirements = image.getMemoryRequirements();
		vk::MemoryAllocateInfo allocInfo(memRequirements.size, FindMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal));
		imageMemory = vk::raii::DeviceMemory(device, allocInfo);
		image.bindMemory(*imageMemory, 0);
	}

	vk::raii::ImageView VulkanUtils::CreateImageView2D(vk::raii::Image &image, vk::Format format)
	{
		auto &device = GraphicsContext::Get().GetDevice();
		vk::ImageViewCreateInfo image_view_create_info({}, image, vk::ImageViewType::e2D, format, {}, {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
		return device.createImageView(image_view_create_info);
	}

	vk::raii::Sampler VulkanUtils::CreateImageSampler(const vk::SamplerCreateInfo &info)
	{
		auto &device = GraphicsContext::Get().GetDevice();
		return device.createSampler(info);
	}

	void VulkanUtils::CopyBuffer(vk::raii::Buffer &srcBuffer, vk::raii::Buffer &dstBuffer, vk::DeviceSize size)
	{
		auto cmd = BeginSingleTimeCommands();
		cmd.copyBuffer(*srcBuffer, *dstBuffer, vk::BufferCopy(0, 0, size));
		EndSingleTimeCommands(cmd);
	}

	void VulkanUtils::TransitionImageLayout(const vk::raii::Image &image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
	{
		auto cmd = BeginSingleTimeCommands();
		vk::ImageMemoryBarrier barrier({}, {}, oldLayout, newLayout, {}, {}, image, {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});

		vk::PipelineStageFlags sourceStage;
		vk::PipelineStageFlags destinationStage;

		if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
		{
			barrier.srcAccessMask = {};
			barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
			sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
			destinationStage = vk::PipelineStageFlagBits::eTransfer;
		}
		else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
		{
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
			sourceStage = vk::PipelineStageFlagBits::eTransfer;
			destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
		}
		else
		{
			LOG_ERROR("Unsupported layout transition!");
			ASSERT(false);
		}

		cmd.pipelineBarrier(sourceStage, destinationStage, {}, {}, nullptr, barrier);
		EndSingleTimeCommands(cmd);
	}

	uint32_t VulkanUtils::FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
	{
		auto &physical_device = GraphicsContext::Get().GetPhysicalDevice();
		auto memoryProperties = physical_device.getMemoryProperties();
		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}

		ASSERT(false, "Failed to find suitable memory type!")
	}

	void VulkanUtils::CopyBufferToImage(vk::raii::Buffer &buffer, vk::raii::Image &image, uint32_t width, uint32_t height)
	{
		auto cmd = BeginSingleTimeCommands();
		vk::BufferImageCopy region(0, 0, 0, {vk::ImageAspectFlagBits::eColor, 0, 0, 1}, {0, 0, 0}, {width, height, 1});
		cmd.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, region);
		EndSingleTimeCommands(cmd);
	}

	void VulkanUtils::SetBufferData(const vk::raii::DeviceMemory &memory, const void *data, vk::DeviceSize size)
	{
		void *stagingData = memory.mapMemory(0, size);
		memcpy(stagingData, data, size);
		memory.unmapMemory();
	}

	vk::raii::ShaderModule VulkanUtils::CreateShaderModule(const vk::ShaderModuleCreateInfo &info)
	{
		auto &device = GraphicsContext::Get().GetDevice();
		return device.createShaderModule(info);
	}

} // namespace BHive