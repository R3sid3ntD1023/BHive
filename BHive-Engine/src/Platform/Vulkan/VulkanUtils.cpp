#include "gfx/RenderCommand.h"
#include "VulkanRendererAPI.h"
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

		auto formatItr = std::ranges::find_if(availableFormats, [](auto f) { return f.format == vk::Format::eR8G8B8A8Unorm && f.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear; });
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

			if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)
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
		auto &cmdPool = VulkanBackend::GetImmediateCommandPool();
		auto &device = VulkanBackend::GetLogicalDevice();

		vk::CommandBufferAllocateInfo allocInfo(cmdPool, vk::CommandBufferLevel::ePrimary, 1);
		vk::raii::CommandBuffer cmdbuffer = std::move(device.allocateCommandBuffers(allocInfo).front());
		cmdbuffer.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
		return cmdbuffer;
	}

	void VulkanUtils::EndSingleTimeCommands(vk::raii::CommandBuffer &commandBuffer)
	{
		commandBuffer.end();

		vk::SubmitInfo submitInfo({}, {}, *commandBuffer);
		auto &graphics_queue = VulkanBackend::GetQueueFamilies().GraphicsQueue;
		graphics_queue.submit(submitInfo, nullptr);
		graphics_queue.waitIdle();
	}

	void VulkanUtils::CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Buffer &buffer)
	{
		auto &device = VulkanBackend::GetLogicalDevice();
		auto &physical_device = VulkanBackend::GetPhysicalDevice();

		vk::DeviceSize atom = physical_device.getProperties().limits.nonCoherentAtomSize;
		vk::DeviceSize requested = size;
		vk::DeviceSize minAlloc = (requested + atom - 1) & ~(atom - 1);

		vk::BufferCreateInfo bufferCreateInfo({}, size, usage, vk::SharingMode::eExclusive);
		buffer = vk::raii::Buffer(device, bufferCreateInfo);
	}

	void VulkanUtils::CreateImage(
		uint32_t w, uint32_t h, uint32_t d, uint32_t layers, vk::ImageType type, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Image &image)
	{
		auto &device = VulkanBackend::GetLogicalDevice();
		vk::ImageCreateInfo imageInfo(
			{}, type, format, {w, h, d}, 1, layers, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, usage,
			vk::SharingMode::eExclusive, 0);
		image = device.createImage(imageInfo);
	}

	void VulkanUtils::CreateImageView(const vk::Image &image, vk::raii::ImageView &view, vk::ImageViewType type, vk::Format format, vk::ImageAspectFlags aspect,
		uint32_t layerCount)
	{
		auto &device = VulkanBackend::GetLogicalDevice();
		vk::ImageSubresourceRange range(aspect, 0, 1, 0, layerCount);
		vk::ImageViewCreateInfo image_view_create_info({}, image, type, format, {}, range);
		view = device.createImageView(image_view_create_info);
	}

	void VulkanUtils::CreateImageSampler(vk::raii::Sampler& sampler, const vk::SamplerCreateInfo &info)
	{
		auto &device = VulkanBackend::GetLogicalDevice();
		sampler = device.createSampler(info);
	}

	void VulkanUtils::CopyBuffer(const vk::raii::Buffer &srcBuffer, vk::raii::Buffer &dstBuffer, vk::DeviceSize size)
	{
		auto cmd = BeginSingleTimeCommands();
		cmd.copyBuffer(srcBuffer, dstBuffer, vk::BufferCopy(0, 0, size));
		EndSingleTimeCommands(cmd);
	}

	void VulkanUtils::TransitionImageLayout(const vk::Image &image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
	{
		auto cmd = BeginSingleTimeCommands();

		vk::PipelineStageFlags src_stage;
		vk::PipelineStageFlags dst_stage;
		vk::AccessFlags src_access_mask;
		vk::AccessFlags dst_access_mask;

		if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
		{
			src_access_mask = vk::AccessFlagBits::eNone;
			dst_access_mask = vk::AccessFlagBits::eTransferWrite;
			src_stage = vk::PipelineStageFlagBits::eTopOfPipe;
			dst_stage = vk::PipelineStageFlagBits::eTransfer;
		}
		else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
		{
			src_access_mask = vk::AccessFlagBits::eTransferWrite;
			dst_access_mask = vk::AccessFlagBits::eShaderRead;
			src_stage = vk::PipelineStageFlagBits::eTransfer;
			dst_stage = vk::PipelineStageFlagBits::eFragmentShader;
		}
		else
		{
			LOG_ERROR("Unsupported layout transition!");
			ASSERT(false);
		}

		vk::ImageSubresourceRange range(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 );
		vk::ImageMemoryBarrier barrier(src_access_mask, dst_access_mask, oldLayout, newLayout, {}, {}, image, range);
		cmd.pipelineBarrier(src_stage, dst_stage, {}, {}, nullptr, barrier);
		EndSingleTimeCommands(cmd);
	}

	void VulkanUtils::TransitionImageLayout(
		vk::raii::CommandBuffer &cmd, const vk::Image &image,  vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::AccessFlags2 srcAccessMask, vk::AccessFlags2 dstAccessMask,
		vk::PipelineStageFlags2 srcStageMask, vk::PipelineStageFlags2 dstStageMask, vk::ImageAspectFlags aspect_flags, const ImageSubresource &sub)
	{
		bool valid = oldLayout == vk::ImageLayout::eUndefined &&
			newLayout == vk::ImageLayout::eShaderReadOnlyOptimal;
		if (valid)
			__debugbreak();

		vk::ImageSubresourceRange range{aspect_flags, sub.MipLevel, 1, sub.BaseArrayLayer, sub.LayerCount};
		vk::ImageMemoryBarrier2 barrier(
			srcStageMask,
			srcAccessMask, 
			dstStageMask, 
			dstAccessMask, 
			oldLayout, 
			newLayout, 
			VK_QUEUE_FAMILY_IGNORED,
			VK_QUEUE_FAMILY_IGNORED, 
			image, 
			range);

		vk::DependencyInfo depInfo({}, {}, {}, barrier);
		cmd.pipelineBarrier2(depInfo);
	}

	void VulkanUtils::CopyBufferToImage(const vk::raii::CommandBuffer &cmd, const vk::Buffer &buffer, vk::Image &image, const ImageCopyRegion &region)
	{
		vk::Offset3D offset(region.Offset.x, region.Offset.y, region.Offset.z);
		vk::Extent3D extent(region.Extents.x, region.Extents.y, region.Extents.z);

		vk::ImageSubresourceLayers layers(vk::ImageAspectFlagBits::eColor, 0, region.BaseArrayLayer, region.LayerCount);
		vk::BufferImageCopy image_region(0, 0, 0, layers, offset, extent);
		cmd.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, image_region);
	}

	void VulkanUtils::SetBufferData(const vk::raii::DeviceMemory &memory, const void *data, vk::DeviceSize size)
	{
		void *stagingData = memory.mapMemory(0, size);
		memcpy(stagingData, data, size);
		memory.unmapMemory();
	}

	vk::Format VulkanUtils::FindDepthFormat()
	{
		auto physical_device = VulkanBackend::GetPhysicalDevice();
		return FindSupportedFormat(
			physical_device, {vk::Format::eD24UnormS8Uint, vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint}, vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
	}

	bool VulkanUtils::HasStencilComponent(vk::Format format)
	{
		return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
	}

} // namespace BHive