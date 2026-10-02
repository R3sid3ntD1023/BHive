#pragma once

#include "GPUResourceHandle.h"
#include "VulkanCore.h"
#include "gfx/Enumerations.h"
#include "gfx/NativeHandle.h"

namespace BHive
{
	struct VulkanBuffer : public INativeObject
	{
		~VulkanBuffer();

		void Initialize(size_t size, vk::BufferUsageFlags usage, EBufferLifetime lifeTime);

		GPUBufferResourceHandle GetNative(uint32_t frame = 0) const;

		void Upload(const void *data, size_t size, uint32_t offset);

		void ClearData();

	private:
		void InitStatic(size_t size, vk::BufferUsageFlags usage);
		void InitDynamic(size_t size, vk::BufferUsageFlags usage);

	private:
		std::array<GPUBufferResourceHandle, MAX_FRAMES_IN_FLIGHT> mBuffers;
		std::array<void *, MAX_FRAMES_IN_FLIGHT> mMappedPtrs{};
		EBufferLifetime mLifeTime{};
	};
} // namespace BHive