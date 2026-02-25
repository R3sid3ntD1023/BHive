#pragma once

#include <vulkan/vulkan_raii.hpp>
#include "gfx/NativeHandle.h"
#include "core/UUID.h"
#include "VulkanImageRegions.h"

namespace BHive
{
	struct ImageSubresource;
	class GPUResourceManager;

	namespace Vulkan
	{
		
		struct ImageState
		{
			vk::ImageLayout Layout = vk::ImageLayout::eUndefined;
			vk::AccessFlags2 Access = {};
			vk::PipelineStageFlags2 Stage = {};
		};

		struct Image
		{
			void SetImage(const vk::Image &img) { ImageSrc = img;}
			
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
			vk::Image& GetImage()  { return Image; }

			vk::ImageView& GetView() { return View; }

			vk::Sampler& GetSampler() { return Sampler; }

			const vk::DescriptorImageInfo GetDescriptor() const;

			void Transition(vk::raii::CommandBuffer &cmd, const ImageState &newState, const ImageSubresource &sub = {0, 0, 1});

		private:

			vk::Image Image = VK_NULL_HANDLE;
			vk::DeviceMemory Memory = VK_NULL_HANDLE;
			vk::ImageView View = VK_NULL_HANDLE;
			vk::Sampler Sampler = VK_NULL_HANDLE;
			std::vector<ImageState> LayerStates;
			vk::ImageAspectFlags Aspect;
			uint32_t ArrayLayers = 1;
			UUID Handle;

			friend GPUResourceManager;
		};

		struct AllocatedBuffer
		{
			vk::Buffer Buffer = VK_NULL_HANDLE;
			vk::DeviceMemory Memory = VK_NULL_HANDLE;
			UUID Handle;
		};

		struct Handle 
		{
			static NativeHandle BufferInfo(const vk::DescriptorBufferInfo *info) { return NativeHandle::FromPtr(info); }
			static NativeHandle ImageInfo(const vk::DescriptorImageInfo *info) { return NativeHandle::FromPtr(info); }
			static NativeHandle Buffer(const vk::raii::Buffer *buffer) { return NativeHandle::FromPtr(buffer); }
		};
		
	}
} // namespace BHive