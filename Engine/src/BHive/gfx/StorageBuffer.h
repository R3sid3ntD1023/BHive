#pragma once

#include "core/Core.h"
#include "BufferBase.h"

namespace BHive
{
	class StorageBuffer : public BufferBase
	{
	public:
		StorageBuffer(uint32_t binding, size_t size);
		StorageBuffer(const void *data, size_t size);
		StorageBuffer(size_t size);

		virtual ~StorageBuffer() = default;

		virtual void BindBufferBase(uint32_t binding) const override;
	};

} // namespace BHive
