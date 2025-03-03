#pragma once

#include "core/Core.h"

namespace BHive
{
	class GLBufferBase
	{
	public:
		GLBufferBase();
		virtual ~GLBufferBase();

		void SetData(const void *data, size_t size, uint32_t offset);

		uint32_t mBufferID = 0;
	};
} // namespace BHive