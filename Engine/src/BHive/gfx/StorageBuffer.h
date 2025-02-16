#pragma once

#include "core/Core.h"

namespace BHive
{
	class StorageBuffer
	{
	private:
		/* data */
	public:
		virtual ~StorageBuffer() = default;
		virtual uint32_t GetRendererID() const = 0;
		virtual void SetData(const void *data, size_t size, uint32_t offset = 0) = 0;

		static Ref<StorageBuffer> Create(uint32_t binding, size_t size);
	};

} // namespace BHive
