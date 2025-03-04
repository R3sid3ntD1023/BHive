#pragma once

#include "core/Core.h"

namespace BHive
{
	class TransformFeedback
	{
	public:
		TransformFeedback(size_t size);
		virtual ~TransformFeedback();

		virtual uint32_t GetRendererID() const { return mTransformFeedback; }
		virtual void GetData(void *data, size_t size);
		virtual void Begin(uint32_t mode);
		virtual void End();

	private:
		uint32_t mTransformFeedback = 0;
	};

} // namespace BHive
