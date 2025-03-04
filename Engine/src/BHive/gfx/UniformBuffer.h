#pragma once

#include "BufferBase.h"

namespace BHive
{
	class UniformBuffer : public BufferBase
	{
	public:
		UniformBuffer(uint32_t binding, uint64_t size);
		UniformBuffer(uint64_t size, const void *data);

		virtual void BindBufferBase(uint32_t binding) const override;
	};
} // namespace BHive