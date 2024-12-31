#pragma once

#include "gfx/UniformBuffer.h"

namespace BHive
{
	class GLUniformBuffer : public UniformBuffer
	{
	public:
		GLUniformBuffer(uint32_t binding, uint64_t size);
		GLUniformBuffer(uint64_t size, const void* data);
		~GLUniformBuffer();

		void Bind(uint32_t binding) const;
		void SetData(const void* data, uint64_t size, uint32_t offset = 0);

	private:
		uint32_t mUniformBufferID{ 0 };
	};
}