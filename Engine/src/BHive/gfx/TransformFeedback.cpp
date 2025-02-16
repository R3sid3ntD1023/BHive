#include "TransformFeedback.h"
#include "Platform/GL/GLTransformFeedback.h"

namespace BHive
{
	Ref<TransformFeedback> BHive::TransformFeedback::Create(size_t size)
	{
		return CreateRef<GLTransformFeedback>(size);
	}
} // namespace BHive