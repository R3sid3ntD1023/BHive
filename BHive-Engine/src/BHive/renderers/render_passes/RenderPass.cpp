#include "gfx/Framebuffer.h"
#include "RenderPass.h"

namespace BHive
{
	void RenderPass::CreateResizableObjects(const glm::uvec2 &size)
	{
		mSize = size;
		CreateFramebuffer();
	}

	void RenderPass::Resize(const glm::uvec2 &size)
	{
		mSize = size;

		if (mFrambuffer)
			mFrambuffer->Resize(size);
	}

	bool RenderPass::IsEnabled() const
	{
		return mSize.x > 0 && mSize.y > 0;
	}
} // namespace BHive