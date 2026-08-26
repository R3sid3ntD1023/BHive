#pragma once

#include "ResourceHandle.h"

namespace BHive
{
	struct IResourceRegistry
	{
		virtual ~IResourceRegistry() = default;

		virtual void Destroy(ResourceHandle) = 0;

		virtual bool IsValid(ResourceHandle) const = 0;

		virtual void *GetRaw(ResourceHandle) const = 0;

		template <typename U>
		U *Get(ResourceHandle h) const
		{
			return reinterpret_cast<U *>(GetRaw(h));
		}
	};

} // namespace BHive