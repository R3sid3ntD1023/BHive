#pragma once

#include "core/Core.h"

namespace BHive
{
	class BHIVE UniformBuffer
	{
	public:
		virtual void Bind(uint32_t binding) const = 0;

		virtual void SetData(const void *data, uint64_t size, uint32_t offset = 0) = 0;

		template <typename T>
		void SetData(const T &data, uint32_t offset = 0)
		{
			SetData(&data, sizeof(T), offset);
		}

		template <typename T>
		void SetData(const std::vector<T> &data, uint32_t offset = 0)
		{
			SetData(data.data(), data.size() * sizeof(T), offset);
		}

		static Ref<UniformBuffer> Create(uint32_t binding, uint64_t size);
		static Ref<UniformBuffer> Create(uint64_t size, const void *data);
	};
} // namespace BHive