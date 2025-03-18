#pragma once

#include "sprite/FlipBook.h"
#include "Component.h"

namespace BHive
{
	struct FlipBookComponent : public Component
	{
		bool AutoPlay{true};

		FColor Color{0xffffffff};

		glm::vec2 Size{1, 1};

		glm::vec2 Tiling{1, 1};

		TAssetHandle<FlipBook> FlipBook;

		virtual void Begin() override;
		virtual void Update(float dt) override;
		virtual void Render() override;

		REFLECTABLEV(Component)
	};

} // namespace BHive