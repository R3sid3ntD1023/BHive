#pragma once

#include "core/Core.h"
#include "gfx/registries/Handles.h"

namespace BHive
{
	struct IBindingGroup
	{
		virtual ~IBindingGroup() = default;
		virtual void SetBuffer(uint32_t binding, BufferPtr buffer) = 0;
		virtual void SetTexture(uint32_t binding, TexturePtr texture, uint32_t mip = 0) = 0;
		virtual uint32_t GetSetIndex() const = 0;
	};
} // namespace BHive