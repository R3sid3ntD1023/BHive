#pragma once

#include "core/Core.h"

namespace BHive
{
	class Texture;

	class PostProcessRenderPass
	{
	public:
		virtual ~PostProcessRenderPass() = default;

		virtual void Init() {};

		virtual void CreateResizableObjects(const glm::uvec2 &size);

		virtual void Process(const Ref<Texture> &texture) = 0;

		virtual void Resize(const glm::uvec2 &size);

		virtual Ref<Texture> GetOutputTexture() const = 0;

		virtual bool IsEnabled() { return mSize.x > 0 && mSize.y > 0; }

	protected:
		glm::uvec2 mSize{0, 0};
	};
} // namespace BHive