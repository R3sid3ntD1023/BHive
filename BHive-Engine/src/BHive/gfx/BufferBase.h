#pragma once

#include "core/Core.h"

namespace BHive
{
	class BHIVE_API BufferBase
	{
	public:
		virtual ~BufferBase() = default;

		virtual void SetData(const void *data, size_t size, uint32_t offset = 0) = 0;

		virtual uintptr_t GetNativeHandle() const = 0;
	};
} // namespace BHive