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

		Ref<FlipBook> FlipBook;

		virtual void Begin() override;
		virtual void Update(float dt) override;
		virtual void Render() override;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(Component)
	};

} // namespace BHive