#pragma once

#include "core/Core.h"
#include "gfx/NativeHandle.h"

namespace BHive
{
	class BufferBase;
	class Texture;

	struct GlobalBuffers
	{
		
		void Register(uint32_t binding, const Ref<BufferBase> & buffer);

		void Register(uint32_t binding, const Ref<Texture>& texture);

		const Ref<BufferBase> GetBuffer(uint32_t binding) const { return mBuffers.at(binding); }

		bool Contains(uint32_t binding) const;

		auto& GetBuffers() const { return mBuffers; }

		auto& GetTextures() const { return mTextures; }

	private:
		std::unordered_map<uint32_t, Ref<BufferBase>> mBuffers;
		std::unordered_map<uint32_t, Ref<Texture>> mTextures;
	};

}