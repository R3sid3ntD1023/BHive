#pragma once

#include "gfx/NativeHandle.h"
#include "core/UUID.h"
#include "ImageState.h"
#include "MemoryAllocator.h"
#include "ImageComponent.h"
#include "VulkanImageRegions.h"

namespace BHive
{
	struct ImageViewDesc;
	class GPUResourceManager;


	struct GPUImage
	{
		UUID ImageHandle = NullID;

		MemoryAllocation Allocation;

		vk::ImageAspectFlags Aspect;

		vk::ImageUsageFlags Usage;

		uint32_t ArrayLayers = 1;

		uint32_t MipLevels = 1;

		std::string DebugName;

		std::unordered_map<size_t, Scope<IImageComponent>> Components;

		GPUImage() = default;

		GPUImage(const GPUImage &) = delete;

		GPUImage &operator=(const GPUImage &) = delete;

		GPUImage(GPUImage &&) = default;

		GPUImage &operator=(GPUImage &&) = default;

		const vk::Image &GetImage() const;

		void Transition(vk::raii::CommandBuffer &cmd, const ImageState &newState, const ImageSubresource &sub = {0, 0, 1});

		vk::ImageView GetView(uint32_t layer, uint32_t face, uint32_t mip);

		template <typename T, typename... TArgs>
		T *AddComponent(TArgs &&...args)
		{
			auto component = CreateScope<T>(std::forward<TArgs>(args)...);
			T *ptr = component.get();
			Components[typeid(T).hash_code()] = std::move(component);
			return ptr;
		}

		template <typename T>
		T *GetComponent() const
		{
			size_t id = typeid(T).hash_code();
			if (Components.contains(id))
				return static_cast<T *>(Components.at(id).get());
			return nullptr;
		}
	};	

	struct AllocatedBuffer
	{
		UUID Buffer = NullID;

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