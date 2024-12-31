#pragma once

#include "core/Core.h"

namespace BHive
{
	class Texture;

	class TextureHandle
	{
	public:
		virtual ~TextureHandle() = default;

		virtual uint64_t GetResourceHandle() const = 0;

		static Ref<TextureHandle> Create(const Ref<Texture>& texture);
	};
}