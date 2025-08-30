#pragma once

#include "core/Core.h"
#include "renderers/RenderData.h"

namespace BHive
{
	class Framebuffer;

	class RenderPass
	{
	public:
		virtual void Init();

		virtual void CreateResizableObjects(const glm::uvec2 &size);

		virtual void Resize(const glm::uvec2 &size);

		virtual void Render(const FMeshRenderDatas &data) = 0;

		virtual void CreateFramebuffer();

		virtual bool IsEnabled() const;

	protected:
		Ref<Framebuffer> mFrambuffer;
		glm::uvec2 mSize{0, 0};
	};
} // namespace BHive