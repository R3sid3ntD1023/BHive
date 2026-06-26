#pragma once

#include "Core/Core.h"
#include "VulkanCore.h"
#include "gfx/resources/ImageCopyRegion.h"
#include "gfx/resources/ImageSubResourceRange.h"

namespace BHive
{

	struct VulkanUtils
	{
		static uint32_t FindQueueFamilies(vk::PhysicalDevice device);

		static vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats);

		static vk::PresentModeKHR ChooseSwapPresentMode(vk::PresentModeKHR requested, const std::vector<vk::PresentModeKHR> &availablePresentModes);

		static vk::Format FindSupportedFormat(vk::PhysicalDevice device, const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlagBits features);

		static vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities, uint32_t w, uint32_t h);

		static uint32_t ChooseMinImageCount(vk::SurfaceCapabilitiesKHR capabilities);

		static vk::raii::CommandBuffer BeginSingleTimeCommands();

		static void EndSingleTimeCommands(vk::raii::CommandBuffer &commandBuffer);

		static void CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Buffer &buffer);

		static void CopyBuffer(const vk::raii::Buffer &srcBuffer, vk::raii::Buffer &dstBuffer, vk::DeviceSize size);

		static void TransitionImageLayout(
			vk::raii::CommandBuffer& cmd, vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::AccessFlags2 srcAccessMask, vk::AccessFlags2 dstAccessMask,
			vk::PipelineStageFlags2 srcStageMask, vk::PipelineStageFlags2 dstStageMask, vk::ImageAspectFlags aspect_flags, ImageSubresourceRange range);

		static void CopyBufferToImage(vk::raii::CommandBuffer &cmd, vk::Buffer buffer, vk::Image image, ImageCopyRegion region);

		static void SetBufferData(const vk::raii::DeviceMemory &memory, const void *data, vk::DeviceSize size);

		static vk::Format FindDepthFormat();

		static bool HasStencilComponent(vk::Format format);
	};

	class SingleTimeCommand
	{
	public:
		SingleTimeCommand();

		~SingleTimeCommand();

		operator vk::raii::CommandBuffer &() { return mCommandBuffer; }

		operator vk::CommandBuffer () { return mCommandBuffer; }

		vk::raii::CommandBuffer& Get() { return mCommandBuffer; }

	private:
		vk::raii::CommandBuffer mCommandBuffer = VK_NULL_HANDLE;
	};
} // namespace BHive