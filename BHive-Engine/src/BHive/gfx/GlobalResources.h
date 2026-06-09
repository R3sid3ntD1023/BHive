#pragma once

#include "core/Core.h"
#include "renderers/FrameGlobals.h"
#include "gfx/Buffers.h"

namespace BHive
{
	class BufferBase;
	class Texture;

	struct GlobalResources
	{
		void Register(uint32_t binding, const Ref<BufferBase> &buffer) { mBuffers.emplace(binding, buffer); }

		void Register(uint32_t binding, const Ref<Texture> &texture) { mTextures.emplace(binding, texture); }

		auto& GetBuffers() const { return mBuffers; }

		auto& GetTextures() const { return mTextures; }

		auto &GetBuffer(uint32_t binding) const { return mBuffers.at(binding); }

		auto &GetTexture(uint32_t binding) const { return mTextures.at(binding); }

		static GlobalResources &Get()
		{
			static GlobalResources globals;
			return globals;
		}

	private:
		GlobalResources()
		{ 
			//register camera buffer
			Register(0, GPUBuffer::Create(sizeof(FCameraData), EBufferType::UniformBuffer));
		}

		std::unordered_map<uint32_t, Ref<BufferBase>> mBuffers;
		std::unordered_map<uint32_t, Ref<Texture>> mTextures;
	};

}