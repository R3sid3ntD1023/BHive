#pragma once

#include "Core/Core.h"
#include "VulkanCore.h"

namespace BHive
{
	struct VulkanUtils
	{
		static vk::raii::CommandBuffer BeginSingleTimeCommands();

		static void EndSingleTimeCommands(vk::raii::CommandBuffer &commandBuffer);

		static void CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Buffer &buffer, vk::raii::DeviceMemory &bufferMemory);

		static void CreateImage2D(
			uint32_t w, uint32_t h, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Image &image,
			vk::raii::DeviceMemory &imageMemory);

		static vk::raii::ImageView CreateImageView2D(vk::raii::Image &image, vk::Format format);

		static vk::raii::Sampler CreateImageSampler(const vk::SamplerCreateInfo &info);

		static void CopyBuffer(vk::raii::Buffer &srcBuffer, vk::raii::Buffer &dstBuffer, vk::DeviceSize size);

		static void TransitionImageLayout(const vk::raii::Image &image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

		static uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

		static void CopyBufferToImage(vk::raii::Buffer &buffer, vk::raii::Image &image, uint32_t width, uint32_t height);

		static void SetBufferData(const vk::raii::DeviceMemory &memory, const void *data, vk::DeviceSize size);

		static vk::raii::ShaderModule CreateShaderModule(const vk::ShaderModuleCreateInfo &info);
	};
} // namespace BHive