#pragma once

#include "gfx/Framebuffer.h"
#include "IResourceFactory.h"

namespace BHive
{
	struct BHIVE_API FramebufferFactory : public IResourceFactory<Framebuffer>
	{
		static FramebufferPtr Create(const FramebufferSpecification &specification);
	};
}; // namespace BHive