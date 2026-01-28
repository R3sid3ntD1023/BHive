#pragma once

#include "core/Core.h"

namespace BHive
{
	class Texture;

	class BHIVE_API PostProcessRenderPass
	{
	public:
		virtual ~PostProcessRenderPass() = default;

		virtual void Init() {};

		virtual void CreateResizableObjects(const glm::uvec2 &size);

		virtual void Process(const Ref<Texture> &texture) = 0;

		virtual void Resize(const glm::uvec2 &size);

		virtual Ref<Texture> GetOutputTexture() const = 0;

		void SetEnabled(bool enabled);

		virtual const char *GetName() const = 0;

		virtual bool IsEnabled() const { return mSize.x > 0 && mSize.y > 0 && mEnabled; }

	protected:
		glm::uvec2 mSize{0, 0};
		bool mEnabled{true};
	};
} // namespace BHive