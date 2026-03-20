#include "PostProcessRenderPass.h"

namespace BHive
{
	void PostProcessRenderPass::Resize(const glm::uvec2 &size)
	{
		CreateResizableObjects(size);
	}

	void PostProcessRenderPass::CreateResizableObjects(const glm::uvec2 &size)
	{
		mSize = size;
	}

	void PostProcessRenderPass::SetEnabled(bool enabled)
	{
		mEnabled = enabled;
	}
} // namespace BHive