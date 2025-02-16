#pragma once

#include "gfx/TransformFeedback.h"

namespace BHive
{
	class GLTransformFeedback : public TransformFeedback
	{
	private:
		/* data */
	public:
		GLTransformFeedback(size_t size);
		~GLTransformFeedback();

		virtual uint32_t GetRendererID() const { return mTransformFeedbackID; };
		virtual void GetData(void *data, size_t size);
		virtual void Begin(uint32_t mode);
		virtual void End();

	private:
		uint32_t mTransformFeedbackID = 0;
	};

} // namespace BHive
