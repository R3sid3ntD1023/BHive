#pragma once

#include "core/Core.h"
#include "NativeHandle.h"

namespace BHive
{
	class BufferBase;
	class Texture;

	class ISetManager
	{
	public:
		virtual ~ISetManager() = default;

		virtual void SetBuffer(uint32_t binding, const Ref<BufferBase> &buffer) = 0;

		virtual void SetTexture(uint32_t binding, const Ref<Texture> &texture, uint32_t mip = 0) = 0;

		virtual void Update(uint32_t frame) = 0;

		virtual NativeHandle GetNativeSet(uint32_t frame) = 0;
	};
}