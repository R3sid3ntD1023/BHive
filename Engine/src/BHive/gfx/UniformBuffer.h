#pragma once

#include "BufferBase.h"

namespace BHive
{
	class BHIVE_API UniformBuffer : public BufferBase
	{
	public:
		virtual ~UniformBuffer() = default;

		virtual void SetData(const void *data, size_t size, uint32_t offset = 0) = 0;

		virtual uintptr_t GetNativeHandle() const = 0;

		static Ref<UniformBuffer> Create(uint32_t binding, uint64_t size, const void *data = nullptr);
	};
} // namespace BHive