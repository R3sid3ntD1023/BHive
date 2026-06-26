#pragma once

#include "core/Core.h"

namespace BHive
{
	struct IRendererContext
	{
		virtual ~IRendererContext() = default;

		template <typename TRendererContext>
			requires(std::is_base_of_v<IRendererContext, TRendererContext>)
		TRendererContext &As()
		{
			ASSERT(dynamic_cast<TRendererContext *>(this) != nullptr)
			return static_cast<TRendererContext &>(*this);
		}
	};
}