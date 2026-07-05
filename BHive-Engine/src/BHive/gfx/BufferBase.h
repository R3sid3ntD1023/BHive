#pragma once

#include "core/Core.h"
#include "NativeHandle.h"
#include "gfx/Enumerations.h"

namespace BHive
{
	

	class BHIVE_API BufferBase 
	{
	public:
		virtual ~BufferBase() = default;

		void SetData(const void *data, size_t size, uint32_t offset = 0);

		virtual NativeHandle GetNativeHandle() const = 0;

		virtual bool NeedsBarrier() const { return false; }
	};
} // namespace BHive