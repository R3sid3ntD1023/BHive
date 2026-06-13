#pragma once

#include "VulkanCore.h"

namespace BHive
{
	struct DescriptorPoolManager
	{
		vk::DescriptorPool GetPool(uint32_t set, uint32_t frame) const;

		void Init(vk::Device device);

		void ResetFrame(uint32_t frameIndex);

		void Shutdown();

	private:
		vk::Device Device = VK_NULL_HANDLE;

		vk::DescriptorPool MaterialPool = VK_NULL_HANDLE;

		vk::DescriptorPool EnginePool = VK_NULL_HANDLE;

		std::array<vk::DescriptorPool, MAX_FRAMES_IN_FLIGHT> FramePools{VK_NULL_HANDLE};
	};
}