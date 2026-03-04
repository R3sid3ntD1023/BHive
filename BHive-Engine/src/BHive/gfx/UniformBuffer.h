#pragma once

#include "BufferBase.h"

namespace BHive
{
	class BHIVE_API UniformBuffer : public BufferBase
	{
	public:
		virtual ~UniformBuffer() = default;

		virtual Type GetType() const override { return BufferBase::UniformBuffer; }

		static Ref<UniformBuffer> Create(uint32_t binding, uint64_t size, const void *data = nullptr);
	};
} // namespace BHive