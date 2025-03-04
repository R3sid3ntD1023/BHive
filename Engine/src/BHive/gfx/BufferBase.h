#pragma once

#include "core/Core.h"

namespace BHive
{
	class BufferBase
	{
	public:
		BufferBase();

		virtual ~BufferBase();

		void SetData(const void *data, size_t size, uint32_t offset = 0);

		virtual void BindBufferBase(uint32_t binding) const;

		uint32_t GetBufferID() const { return mBufferID; }

	protected:
		uint32_t mBufferID = 0;
	};
} // namespace BHive