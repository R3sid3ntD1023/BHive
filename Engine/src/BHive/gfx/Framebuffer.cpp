#include "Framebuffer.h"
#include "Platform/GL/GLFramebuffer.h"

namespace BHive
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification &specification)
	{
		return CreateRef<GLFramebuffer>(specification);
	}

} // namespace BHive