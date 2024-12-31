#include "GraphicsContext.h"
#include "Platform/GL/GLGraphicsContext.h"

namespace BHive
{
	Scope<GraphicsContext> GraphicsContext::Create(void* window)
	{
		return CreateScope<GLGraphicsContext>(window);
	}
}