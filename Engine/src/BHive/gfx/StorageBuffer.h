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

		virtual void BindBufferBase(size_t binding) = 0;
		virtual uint32_t GetRendererID() const = 0;
		virtual void SetData(const void *data, size_t size, uint32_t offset = 0) = 0;

		static Ref<StorageBuffer> Create(uint32_t binding, size_t size);
		static Ref<StorageBuffer> Create(size_t size);
		static Ref<StorageBuffer> Create(const void *data, size_t size);
	};

} // namespace BHive
