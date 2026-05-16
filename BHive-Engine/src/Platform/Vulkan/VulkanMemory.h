#pragma once

#include "gfx/NativeHandle.h"
#include "ResourceID.h"
#include "ImageState.h"
#include "MemoryAllocator.h"
#include "VulkanImageRegions.h"

namespace BHive
{
	struct ImageViewDesc;

	struct ImageViews
	{
		ResourceID Default; //full view : all layers, all ,mips

		//[layer][mip]
		std::vector<std::vector<ResourceID>> Mips;

		//[layer][face][mip] for cube/cubearrays
		std::vector < std::vector < std::vector<ResourceID>>> Faces;

		//[layer][mip] cube view type (optional)
		std::vector<std::vector<ResourceID>> CubeMips;
	};


	struct ImageStateTracker
	{
		std::vector<std::vector<ImageState>> MipStates;

		void Initialize(uint32_t layers, uint32_t mips, const ImageState &initial);

		ImageState &Get(uint32_t layer, uint32_t mip);
	};

	struct GPUImage
	{
		ResourceID Image{0};

		ImageViews Views;

		std::optional<ResourceID> Sampler;

		ImageStateTracker State;

		MemoryAllocation Allocation;

		vk::ImageAspectFlags Aspect;

		vk::ImageUsageFlags Usage;

		uint32_t ArrayLayers = 1;

		uint32_t MipLevels = 1;

		std::string DebugName;

		GPUImage() = default;

		GPUImage(const GPUImage &) = delete;

		GPUImage &operator=(const GPUImage &) = delete;

		GPUImage(GPUImage &&) = default;

		GPUImage &operator=(GPUImage &&) = default;

		const vk::Image GetImage() const;

		const vk::Sampler GetSampler() const;

		void Transition(vk::raii::CommandBuffer &cmd, const ImageState &newState, const ImageSubresource &sub = {0, 0, 1});

		vk::ImageView GetView(uint32_t layer, uint32_t face, uint32_t mip) const;

		vk::ImageView GetDefaultView() const { return GetView(0, 0, 0); }

		vk::ImageView GetMipView(uint32_t mip) const { return GetView(0, 0, mip); }

		vk::ImageView GetLayerMipView(uint32_t layer, uint32_t mip) const { return GetView(layer, 0, mip); }

		vk::ImageView GetCubeFaceView(uint32_t face, uint32_t mip) const { return GetView(0, face, mip); }
	};	

	struct AllocatedBuffer
	{
		ResourceID Buffer{0};

		MemoryAllocation Allocation;

		vk::DeviceSize Size;

		const vk::Buffer& GetBuffer() const;
	};

	struct Handle
	{
		static NativeHandle Image(const GPUImage *image) { return NativeHandle::FromPtr(image); }
		static NativeHandle Buffer(const AllocatedBuffer *buffer) { return NativeHandle::FromPtr(buffer); }
	};
} // namespace BHive