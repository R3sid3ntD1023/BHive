#pragma once

#include "core/UUID.h"
#include "VulkanMemory.h"

namespace BHive
{
	using BufferHandle = UUID;
	using ImageHandle = UUID;

	struct BufferDesc
	{
		size_t Size = 0;
		vk::BufferUsageFlags Usage{};
		vk::MemoryPropertyFlags MemoryFlags{};
	};

	struct ImageDesc
	{
		uint32_t Width = 0, Height = 0, Depth = 1;
		vk::ImageType Type{};
		vk::ImageTiling Tiling{};
		vk::Format Format{};
		vk::ImageUsageFlags Usage{};
		vk::MemoryPropertyFlags MemoryFlags{};
		uint32_t ArrayLayers = 1;
	};

	struct ImageViewDesc
	{
		vk::ImageViewType Type;
		vk::Format Format;
		vk::ImageAspectFlags Aspect;
		uint32_t ArrayLayers = 1;
	};

	

	class GPUResourceManager
	{
	public:

		void Shutdown();

		Vulkan::AllocatedBuffer CreateBuffer(const BufferDesc& desc);

		Vulkan::AllocatedImage CreateImage(const ImageDesc& desc);

		void *MapMemory(const Vulkan::AllocatedBuffer &buffer, vk::DeviceSize offset, vk::DeviceSize size);

		void CreateImageView(Vulkan::Image &image, const ImageViewDesc &desc);

		void CreateImageView(Vulkan::AllocatedImage& image, const ImageViewDesc& desc);

		void CreateSampler(Vulkan::AllocatedImage& image, const vk::SamplerCreateInfo &create_info);

		void DestroyBuffer(Vulkan::AllocatedBuffer buffer);

		void DestroyImage(Vulkan::AllocatedImage image);

		static GPUResourceManager &Get();
	};
}