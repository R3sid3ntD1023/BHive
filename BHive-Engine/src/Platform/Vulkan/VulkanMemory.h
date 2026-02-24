#pragma once

#include <vulkan/vulkan_raii.hpp>
#include "gfx/NativeHandle.h"
#include "core/UUID.h"
#include "VulkanImageRegions.h"

namespace BHive
{
	struct ImageSubresource;

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
			vk::Image ImageSrc = VK_NULL_HANDLE;
			vk::raii::ImageView View = VK_NULL_HANDLE;
			ImageState State;
			vk::ImageAspectFlags Aspect;

			void Transition(vk::raii::CommandBuffer &cmd, const ImageState &newState);
		};

		struct AllocatedImage
		{
			vk::Image Image = VK_NULL_HANDLE;
			vk::DeviceMemory Memory = VK_NULL_HANDLE;
			UUID Handle;
			vk::ImageView View = VK_NULL_HANDLE;
			vk::Sampler Sampler = VK_NULL_HANDLE;
			ImageState State;
			vk::ImageAspectFlags Aspect;
			uint32_t ArrayLayers = 1;

			void Transition(vk::raii::CommandBuffer &cmd, const ImageState &newState, const ImageSubresource &sub = {0, 0, UINT32_MAX});
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