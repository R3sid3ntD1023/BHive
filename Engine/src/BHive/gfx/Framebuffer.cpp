#include "Framebuffer.h"
#include "Platform/GL/GLFramebuffer.h"


namespace BHive
{
	bool Framebuffer::SaveToImage(Framebuffer& framebuffer, const std::filesystem::path& path)
	{
		return framebuffer.SaveToImage(path);
	}

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& specification)
	{
		return CreateRef<GLFramebuffer>(specification);
	}

}