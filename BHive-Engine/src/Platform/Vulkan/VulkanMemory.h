#pragma once

#include "gfx/NativeHandle.h"
#include "gfx/ResourceID.h"
#include "ImageState.h"
#include "MemoryAllocator.h"

namespace BHive
{
	struct ImageViews
	{
		ResourceID Default; // full view : all layers, all ,mips

		//[layer][mip]
		std::vector<std::vector<ResourceID>> Mips;

		//[layer][face][mip] for cube/cubearrays
		std::vector<std::vector<std::vector<ResourceID>>> Faces;

		//[layer][mip] cube view type (optional)
		std::vector<std::vector<ResourceID>> CubeMips;
	};

	struct ImageStateTracker
	{
		std::vector<std::vector<ImageState>> MipStates;

		void Initialize(uint32_t layers, uint32_t mips, const ImageState &initial);

		ImageState &Get(uint32_t layer, uint32_t mip);

		const ImageState &Get(uint32_t layer, uint32_t mip) const;
	};

	struct GPUImage
	{
		~GPUImage();

		ResourceID Image{0};

		ImageViews Views;

		ResourceID Sampler{0};

		vk::ImageUsageFlags Usage;

		vk::ImageViewType ViewType;

		std::string DebugName;

		bool IsCube{false};

		bool IsCubeArray{false};

		const vk::Image GetImage() const;

		const vk::Sampler GetSampler() const;

		vk::ImageView GetView(uint32_t layer, uint32_t face, uint32_t mip) const;

		vk::ImageView GetDefaultView() const { return GetView(0, 0, 0); }

		vk::ImageView GetMipView(uint32_t mip) const { return GetView(0, 0, mip); }

		vk::ImageView GetLayerMipView(uint32_t layer, uint32_t mip) const { return GetView(layer, 0, mip); }

		vk::ImageView GetCubeFaceView(uint32_t face, uint32_t mip) const { return GetView(0, face, mip); }

		bool IsValid() const { return Image; }

		operator bool() const { return IsValid(); }
	};

	struct AllocatedBuffer
	{
		ResourceID Buffer{0};

		vk::DeviceSize Size;

		const MemoryAllocation &GetAllocation() const;

		vk::Buffer GetBuffer() const;
	};
} // namespace BHive