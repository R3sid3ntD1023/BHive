#pragma once

#include "core/Core.h"
#include "CameraBuffer.h"

namespace BHive
{
	struct GlobalBuffers
	{
		static Ref<UniformBuffer> GetUniformBuffer(uint32_t binding) { return sUniformBuffers.at(binding);}

		static void AddGlobalUniformBuffer(uint32_t binding, const Ref<UniformBuffer>& buffer) { sUniformBuffers.emplace(binding, buffer);}

		static void Shutdown() { LOG_TRACE("Global Buffers Shutdown Called") sUniformBuffers.clear(); }

	private:
		static inline std::unordered_map<uint32_t, Ref<UniformBuffer>> sUniformBuffers;
	};
}