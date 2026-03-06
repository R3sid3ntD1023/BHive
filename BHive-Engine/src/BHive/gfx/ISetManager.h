#pragma once

#include "core/Core.h"
#include "NativeHandle.h"
#include "shader/ShaderReflection.h"

namespace BHive
{
	class BufferBase;
	class Texture;

	class ISetManager
	{
	public:
		virtual ~ISetManager() = default;

		virtual void BindBuffer(uint32_t binding, EResourceType type, const Ref<BufferBase> &buffer) = 0;

		virtual void BindSampler(uint32_t binding, EResourceType type, const Ref<Texture> &texture) = 0;

		virtual void Update(uint32_t frame) = 0;

		virtual NativeHandle GetNativeSet(uint32_t frame) = 0;
	};

	class GlobalSetSystem
	{
	public:
		void Register(uint64_t hash, Ref<ISetManager> &manager);

		ISetManager *Get(uint64_t hash) const;

		bool Contains(uint64_t hash) const { return mGlobalManagers.contains(hash); }

	private:
		std::unordered_map<uint64_t, Ref<ISetManager>> mGlobalManagers;
	};
}