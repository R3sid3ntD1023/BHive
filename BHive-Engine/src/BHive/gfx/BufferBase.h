#pragma once

#include "core/Core.h"
#include "gfx/Enumerations.h"

namespace BHive
{

	class BHIVE_API BufferBase
	{
	public:
		virtual ~BufferBase() = default;

		virtual void SetData(const void *data, size_t size, uint32_t offset = 0) = 0;

		virtual void Clear() = 0;

		virtual int32_t GetBufferID() const = 0;

		template <typename T>
		void TSetData(const T &data)
		{
			SetData(&data, sizeof(T), 0);
		}

		template <typename T>
		void TSetData(const T *data, size_t count, uint32_t offset = 0)
		{
			SetData(data, sizeof(T) * count, offset);
		}
	};
} // namespace BHive