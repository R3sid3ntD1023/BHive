#pragma once

#include "core/Core.h"

namespace BHive
{
	class UniformBuffer;
	class StorageBuffer;

	struct GlobalBuffers
	{
		
		static void AddGlobalUniformBuffer(uint32_t binding, const Ref<UniformBuffer>& buffer) { sUniformBuffers.emplace(binding, buffer);}

		static void AddGlobalStorageBuffer(uint32_t binding, const Ref<StorageBuffer> &buffer) { sStorageBuffers.emplace(binding, buffer); }

		static Ref<UniformBuffer> GetUniformBuffer(uint32_t binding) { return sUniformBuffers.at(binding); }

		static Ref<StorageBuffer> GetStorageBuffer(uint32_t binding) { return sStorageBuffers.at(binding); }

		static void Shutdown()
		{
			LOG_TRACE("Global Buffers Shutdown Called");
			sUniformBuffers.clear();
			sStorageBuffers.clear();
		}

	private:
		static inline std::unordered_map<uint32_t, Ref<UniformBuffer>> sUniformBuffers;

		static inline std::unordered_map<uint32_t, Ref<StorageBuffer>> sStorageBuffers;
	};
}