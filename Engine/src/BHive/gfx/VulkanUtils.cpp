#include "GraphicsContext.h"
#include "RenderCommand.h"
#include "VulkanDevice.h"
#include "VulkanUtils.h"

namespace BHive
{
	uint32_t VulkanUtils::FindQueueFamilies(vk::PhysicalDevice device)
	{
		std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();
		auto graphicsQueueFamilyProperty =
			std::find_if(queueFamilies.begin(), queueFamilies.end(), [](const vk::QueueFamilyProperties &qfp) { return (qfp.queueFlags & vk::QueueFlagBits::eGraphics); });

		return static_cast<uint32_t>(std::distance(queueFamilies.begin(), graphicsQueueFamilyProperty));
	}

	vk::SurfaceFormatKHR VulkanUtils::ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats)
	{
		ASSERT(!availableFormats.empty());

		auto formatItr = std::ranges::find_if(availableFormats, [](auto format) { return format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear; });
		return formatItr != availableFormats.end() ? *formatItr : availableFormats[0];
	}

	vk::PresentModeKHR VulkanUtils::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes)
	{
		ASSERT(std::ranges::any_of(availablePresentModes, [](auto mode) { return mode == vk::PresentModeKHR::eFifo; }));
		return std::ranges::any_of(availablePresentModes, [](auto mode) { return mode == vk::PresentModeKHR::eMailbox; }) ? vk::PresentModeKHR::eMailbox : vk::PresentModeKHR::eFifo;
	}

	vk::Format VulkanUtils::FindSupportedFormat(vk::PhysicalDevice device, const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlagBits features)
	{
		for (auto &format : candidates)
		{
			vk::FormatProperties props = device.getFormatProperties(format);
			if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features)
			{
				return format;
			}
			else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)
			{
				return format;
			}
		}

		ASSERT(false, "failed to find supported format!");
	}

	vk::Extent2D VulkanUtils::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities, uint32_t w, uint32_t h)
	{
		if (capabilities.currentExtent.width != 0xFFFFFFF)
		{
			return capabilities.currentExtent;
		}

		vk::Extent2D actualExtent = {w, h};
		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		return actualExtent;
	}

	uint32_t VulkanUtils::ChooseMinImageCount(vk::SurfaceCapabilitiesKHR capabilities)
	{
		auto minImageCount = std::max(3u, capabilities.minImageCount);
		if ((0 < capabilities.maxImageCount) && (capabilities.maxImageCount < minImageCount))
		{
			minImageCount = capabilities.maxImageCount;
		}

		return minImageCount;
	}

	vk::raii::CommandBuffer VulkanUtils::BeginSingleTimeCommands()
	{
		auto api = RenderCommand::GetAPI();
		auto &cmdPool = api->GetCommandPool();
		auto &device = VulkanCore::GetLogicalDevice();

		vk::CommandBufferAllocateInfo allocInfo(cmdPool, vk::CommandBufferLevel::ePrimary, 1);
		vk::raii::CommandBuffer commandBuffer = std::move(device.allocateCommandBuffers(allocInfo).front());
		commandBuffer.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
		return commandBuffer;
	}

	void VulkanUtils::EndSingleTimeCommands(vk::raii::CommandBuffer &commandBuffer)
	{
		commandBuffer.end();

		vk::SubmitInfo submitInfo({}, {}, *commandBuffer);

		auto &graphics_queue = VulkanCore::GetQueueFamilies().GraphicsQueue;
		graphics_queue.submit(submitInfo, nullptr);
		graphics_queue.waitIdle();
	}

	void VulkanUtils::CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Buffer &buffer, vk::raii::DeviceMemory &bufferMemory)
	{
		auto &device = VulkanCore::GetLogicalDevice();
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
		auto &device = VulkanCore::GetLogicalDevice();
		vk::ImageCreateInfo imageInfo(
			{}, vk::ImageType::e2D, format, {w, h, 1}, 1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
			vk::SharingMode::eExclusive, 0);
		image = device.createImage(imageInfo);

		vk::MemoryRequirements memRequirements = image.getMemoryRequirements();
		vk::MemoryAllocateInfo allocInfo(memRequirements.size, FindMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal));
		imageMemory = device.allocateMemory(allocInfo);
		image.bindMemory(*imageMemory, 0);
	}

	vk::raii::ImageView VulkanUtils::CreateImageView2D(vk::raii::Image &image, vk::Format format)
	{
		auto &device = VulkanCore::GetLogicalDevice();
		vk::ImageViewCreateInfo image_view_create_info({}, image, vk::ImageViewType::e2D, format, {}, {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
		return device.createImageView(image_view_create_info);
	}

	vk::raii::Sampler VulkanUtils::CreateImageSampler(const vk::SamplerCreateInfo &info)
	{
		auto &device = VulkanCore::GetLogicalDevice();
		return device.createSampler(info);
	}

	void VulkanUtils::CopyBuffer(vk::raii::Buffer &srcBuffer, vk::raii::Buffer &dstBuffer, vk::DeviceSize size)
	{
		auto cmd = BeginSingleTimeCommands();
		cmd.copyBuffer(*srcBuffer, *dstBuffer, vk::BufferCopy(0, 0, size));
		EndSingleTimeCommands(cmd);
	}

	void VulkanUtils::TransitionImageLayout(const vk::Image &image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
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

	void VulkanUtils::TransitionImageLayout(
		vk::raii::CommandBuffer &cmd, vk::Image &image, uint32_t imageIndex, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::AccessFlags2 srcAccessMask, vk::AccessFlags2 dstAccessMask,
		vk::PipelineStageFlags2 srcStageMask, vk::PipelineStageFlags2 dstStageMask)
	{
		vk::ImageMemoryBarrier2 barrier{};
		barrier.srcStageMask = srcStageMask;
		barrier.srcAccessMask = srcAccessMask;
		barrier.dstStageMask = dstStageMask;
		barrier.dstAccessMask = dstAccessMask;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};

		vk::DependencyInfo depInfo{};
		depInfo.dependencyFlags = {};
		depInfo.imageMemoryBarrierCount = 1;
		depInfo.pImageMemoryBarriers = &barrier;
		cmd.pipelineBarrier2(depInfo);
	}

	uint32_t VulkanUtils::FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
	{
		auto &physical_device = VulkanCore::GetPhysicalDevice();
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

	vk::ShaderModule VulkanUtils::CreateShaderModule(const vk::ShaderModuleCreateInfo &info)
	{
		auto &device = VulkanCore::GetLogicalDevice();
		VkShaderModule module = nullptr;
		VkShaderModuleCreateInfo create_info = info;
		ASSERT(vkCreateShaderModule(*device, &create_info, nullptr, &module) == VK_SUCCESS, "Failed to create shader module!");
		return module;
	}

	vk::Format VulkanUtils::FindDepthFormat(vk::PhysicalDevice physical_device)
	{
		return FindSupportedFormat(
			physical_device, {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint}, vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
	}

	bool VulkanUtils::HasStencilComponent(vk::Format format)
	{
		return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
	}

} // namespace BHive