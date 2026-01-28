#pragma once

#include "core/Core.h"
#include "CameraBuffer.h"

namespace BHive
{
	struct GlobalBuffers
	{
		static inline CameraBuffer CameraData;

		static Ref<UniformBuffer> GetUniformBuffer(uint32_t binding) { return sUniformBuffers.at(binding);}

		static void AddGlobalUniformBuffer(uint32_t binding, const Ref<UniformBuffer>& buffer) { sUniformBuffers.emplace(binding, buffer);}

	private:
		static inline std::unordered_map<uint32_t, Ref<UniformBuffer>> sUniformBuffers;
	};
}