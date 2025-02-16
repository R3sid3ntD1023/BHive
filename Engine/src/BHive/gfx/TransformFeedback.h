#pragma once

#include "core/Core.h"

namespace BHive
{
	class TransformFeedback
	{
	public:
		virtual ~TransformFeedback() = default;
		virtual uint32_t GetRendererID() const = 0;
		virtual void GetData(void *data, size_t size) = 0;
		virtual void Begin(uint32_t mode) = 0;
		virtual void End() = 0;

	public:
		static Ref<TransformFeedback> Create(size_t size);
	};

} // namespace BHive
