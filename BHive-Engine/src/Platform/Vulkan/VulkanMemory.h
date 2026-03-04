#pragma once

#include "gfx/NativeHandle.h"
#include "core/UUID.h"
#include "VulkanImageRegions.h"
#include "MemoryAllocator.h"

namespace BHive
{
	struct ImageSubresource;
	class GPUResourceManager;

	struct ImageState
	{
		vk::ImageLayout Layout = vk::ImageLayout::eUndefined;
		vk::AccessFlags2 Access = {};
		vk::PipelineStageFlags2 Stage = {};
	};

	struct Image
	{
		void SetImage(const vk::Image &img) { ImageSrc = img; }

		vk::ImageView &GetView() { return View; }

		void Transition(vk::raii::CommandBuffer &cmd, const ImageState &newState);

	private:
		vk::Image ImageSrc = VK_NULL_HANDLE;

		vk::ImageView View = VK_NULL_HANDLE;

		vk::ImageAspectFlags Aspect;

		ImageState State = {vk::ImageLayout::eUndefined, {}, vk::PipelineStageFlagBits2::eTopOfPipe};

		friend GPUResourceManager;
	};

	struct AllocatedImage
	{
		vk::Image &GetImage() { return Image; }

		vk::ImageView &GetView() { return View; }

		vk::Sampler &GetSampler() { return Sampler; }

		const vk::Image &GetImage() const { return Image; }

		const vk::ImageView &GetView() const { return View; }

		const vk::Sampler &GetSampler() const { return Sampler; }

		void Transition(vk::raii::CommandBuffer &cmd, const ImageState &newState, const ImageSubresource &sub = {0, 0, 1});

	private:
		vk::Image Image = VK_NULL_HANDLE;
		vk::ImageView View = VK_NULL_HANDLE;
		vk::Sampler Sampler = VK_NULL_HANDLE;
		std::vector<ImageState> LayerStates;
		vk::ImageAspectFlags Aspect;
		uint32_t ArrayLayers = 1;
		MemoryAllocation Allocation;
		UUID Handle;

		friend GPUResourceManager;
	};

	struct AllocatedBuffer
	{
		vk::Buffer Buffer = VK_NULL_HANDLE;
		MemoryAllocation Allocation;
		vk::DeviceSize Size;
		UUID Handle;
	};

	struct Handle
	{
		static NativeHandle Image(const AllocatedImage *image) { return NativeHandle::FromPtr(image); }
		static NativeHandle Buffer(const AllocatedBuffer *buffer) { return NativeHandle::FromPtr(buffer); }
	};
} // namespace BHive