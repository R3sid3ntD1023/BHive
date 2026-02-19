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
		size_t Size = 0;
		uint32_t Width = 0, Height = 0, Depth = 1;
		vk::ImageType Type{};
		vk::ImageTiling Tiling{};
		vk::Format Format{};
		vk::ImageUsageFlags Usage{};
		vk::MemoryPropertyFlags MemoryFlags{};
	};

	struct ImageViewDesc
	{
		vk::ImageViewType Type;
		vk::Format Format;
		vk::ImageAspectFlags Aspect;
	};

	class GPUResourceManager
	{
	public:

		BufferHandle CreateBuffer(const BufferDesc& desc);

		ImageHandle CreateImage(const ImageDesc& desc);

		void CreateImageView(ImageHandle h, const ImageViewDesc& desc);

		void CreateSampler(ImageHandle h, const vk::SamplerCreateInfo &create_info);

		void DestroyBuffer(BufferHandle h);

		void DestroyImage(ImageHandle &h);

		Vulkan::AllocatedBuffer& GetBuffer(BufferHandle &h);

		Vulkan::AllocatedImage& GetImage(ImageHandle &h);

		static GPUResourceManager &Get();

	private:
		std::unordered_map<BufferHandle, Vulkan::AllocatedBuffer> mBuffers;
		std::unordered_map<ImageHandle, Vulkan::AllocatedImage> mImages;
	};
}