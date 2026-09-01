#pragma once

#include "IResourceFactory.h"
#include "gfx/Framebuffer.h"

namespace BHive
{
	struct BHIVE_API FramebufferFactory : public IResourceFactory<Framebuffer>
	{
		static FramebufferPtr Create(const FramebufferSpecification &specification);
	};
}; // namespace BHive