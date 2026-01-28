#pragma once

#include "BufferBase.h"

namespace BHive
{
	class BHIVE_API StorageBuffer : public BufferBase
	{
	public:
		virtual ~StorageBuffer() = default;

		static Ref<StorageBuffer> Create(uint32_t binding, size_t size, const void *data = nullptr);

		static Ref<StorageBuffer> Create(size_t size);
	};

} // namespace BHive
