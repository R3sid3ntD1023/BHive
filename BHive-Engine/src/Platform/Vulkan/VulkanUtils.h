#pragma once

#include "Core/Core.h"
#include "VulkanMemory.h"

namespace BHive
{
	struct VulkanUtils
	{

		static uint32_t FindQueueFamilies(vk::PhysicalDevice device);

		static vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats);

		static vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes);

		static vk::Format FindSupportedFormat(vk::PhysicalDevice device, const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlagBits features);

		static vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities, uint32_t w, uint32_t h);

		static uint32_t ChooseMinImageCount(vk::SurfaceCapabilitiesKHR capabilities);

		static vk::raii::CommandBuffer BeginSingleTimeCommands();

		static void EndSingleTimeCommands(vk::raii::CommandBuffer &commandBuffer);

		static void CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Buffer &buffer, vk::raii::DeviceMemory& memory);

		static void CreateImage(
			uint32_t w, uint32_t h, uint32_t d, vk::ImageType type, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties,
			vk::raii::Image& image, vk::raii::DeviceMemory& memory);

		static void CreateImageView(const vk::raii::Image& image, vk::raii::ImageView& view, vk::ImageViewType type, vk::Format format, vk::ImageAspectFlags aspect);

		static void CreateImageSampler(vk::raii::Sampler& sampler, const vk::SamplerCreateInfo &info);

		static void CopyBuffer(const vk::raii::Buffer &srcBuffer, vk::raii::Buffer &dstBuffer, vk::DeviceSize size);


		static void TransitionImageLayout(const vk::Image &image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

		static void TransitionImageLayout(
			vk::raii::CommandBuffer &cmd, const vk::Image &image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::AccessFlags2 srcAccessMask, vk::AccessFlags2 dstAccessMask,
			vk::PipelineStageFlags2 srcStageMask, vk::PipelineStageFlags2 dstStageMask, vk::ImageAspectFlags aspect_flags);

		static uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

		static void CopyBufferToImage(const vk::Buffer &buffer, vk::Image &image, uint32_t width, uint32_t height);

		static void SetBufferData(const vk::raii::DeviceMemory &memory, const void *data, vk::DeviceSize size);

		static vk::Format FindDepthFormat();

		static bool HasStencilComponent(vk::Format format);
	};
} // namespace BHive