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

		vk::ImageAspectFlags Aspect;

		uint32_t ArrayLayers = 1;

		uint32_t BytesPerPixel = 4;

		uint32_t Size() const { return Width * Height * Depth * BytesPerPixel; }
	};

	struct ImageViewDesc
	{
		vk::ImageViewType Type;

		vk::Format Format;
	};

	class GPUResourceManager
	{
	public:

		void Shutdown();

		AllocatedBuffer CreateBuffer(const BufferDesc& desc);

		AllocatedImage CreateImage(const ImageDesc &desc, const ImageViewDesc &viewDesc);

		void CreateImageView(Image &image, const ImageViewDesc &desc);

		void* MapMemory(AllocatedBuffer &buffer, vk::DeviceSize offset, vk::DeviceSize size);

		void UnmapMemory(AllocatedBuffer &buffer);

		void CreateSampler(AllocatedImage& image, const vk::SamplerCreateInfo &create_info);

		void DestroyBuffer(AllocatedBuffer buffer);

		void DestroyImage(AllocatedImage image);

		void DestroyImage(Image image);
	};
}