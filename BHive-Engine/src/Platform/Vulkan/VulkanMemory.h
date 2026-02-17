#pragma once

#include <vulkan/vulkan_raii.hpp>
#include "gfx/NativeHandle.h"

namespace BHive
{
	namespace Vulkan
	{
		struct Image
		{
			vk::Image ImageSrc = VK_NULL_HANDLE;
			vk::raii::ImageView View = VK_NULL_HANDLE;
			vk::ImageLayout Layout = vk::ImageLayout::eUndefined;
		};

		struct AllocatedImage
		{
			vk::raii::DeviceMemory Memory = VK_NULL_HANDLE;
			vk::raii::Image Image = VK_NULL_HANDLE;
			vk::raii::ImageView View = VK_NULL_HANDLE;
			vk::raii::Sampler Sampler = VK_NULL_HANDLE;
		};

		struct AllocatedBuffer
		{
			vk::raii::Buffer Buffer = VK_NULL_HANDLE;
			vk::raii::DeviceMemory Memory = VK_NULL_HANDLE;
		};

		struct Handle 
		{
			static NativeHandle BufferInfo(const vk::DescriptorBufferInfo *info) { return NativeHandle::FromPtr(info); }
			static NativeHandle ImageInfo(const vk::DescriptorImageInfo *info) { return NativeHandle::FromPtr(info); }
			static NativeHandle Buffer(const vk::raii::Buffer *buffer) { return NativeHandle::FromPtr(buffer); }
		};
		
	}
} // namespace BHive