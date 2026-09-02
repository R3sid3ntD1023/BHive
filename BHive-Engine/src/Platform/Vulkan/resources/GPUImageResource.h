#pragma once

#include "Platform/Vulkan/ImageState.h"
#include "Platform/Vulkan/MemoryAllocator.h"
#include "Platform/Vulkan/VulkanCore.h"
#include "gfx/resources/ImageCopyRegion.h"
#include "gfx/resources/ImageSubResourceRange.h"

namespace BHive
{
	struct ImageInfo
	{
		vk::ImageCreateInfo ImageInfo;
		vk::ImageViewCreateInfo ViewInfo;
		vk::SamplerCreateInfo SamplerInfo;
		vk::MemoryPropertyFlags MemoryFlags;
		uint32_t Layers = 1, MipLevels = 1;
		bool IsCube = false;
	};

	struct GPUImageResource
	{
		struct ViewKey
		{
			uint32_t Layer;
			uint32_t Face;
			uint32_t Mip;

			bool operator<(const ViewKey &other) const
			{
				if (Layer != other.Layer)
					return Layer < other.Layer;
				if (Face != other.Face)
					return Face < other.Face;
				return Mip < other.Mip;
			}
		};

		GPUImageResource(const std::string &name, const ImageInfo &info, MemoryAllocator *allocator, bool external = false);
		~GPUImageResource();

		GPUImageResource(const GPUImageResource &) = delete;
		GPUImageResource &operator=(const GPUImageResource &) = delete;

		void Transition(vk::CommandBuffer cmd, ImageState newState, ImageSubresourceRange subresourceRange);

		void GenerateMipmaps(vk::CommandBuffer cmd, uint32_t layers, glm::uvec2 size);

		ImageState GetImageState(uint32_t layer, uint32_t face, uint32_t mip) const;

		vk::ImageView GetView(uint32_t layer, uint32_t face, uint32_t mip) const;

		ImageState InitFromUsage(vk::Format format, vk::ImageUsageFlags usage);

		void UploadData(vk::CommandBuffer cmd, const void *data, size_t size, ImageCopyRegion region, ImageSubresourceRange subresourceRange);

		vk::raii::Image Image = VK_NULL_HANDLE;
		vk::raii::Sampler Sampler = VK_NULL_HANDLE;
		vk::ImageView View = VK_NULL_HANDLE;
		std::map<ViewKey, vk::raii::ImageView> LayerMipViews;
		std::map<ViewKey, ImageState> LayerMipStates;
		MemoryAllocation Allocation;

	private:
		MemoryAllocator *mAllocator = nullptr;
		vk::Extent3D mExtent;
		vk::ImageAspectFlags mAspectMask;
		vk::ImageCreateInfo mImageCreateInfo;
	};

} // namespace BHive