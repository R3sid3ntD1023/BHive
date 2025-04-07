#pragma once

#include "Component.h"
#include "sprite/FlipBook.h"

namespace BHive
{
	struct FlipBookComponent : public Component
	{
		FlipBookComponent() = default;
		FlipBookComponent(const FlipBookComponent &) = default;

		bool AutoPlay{true};

		FColor Color{0xffffffff};

		glm::vec2 Size{1, 1};

		glm::vec2 Tiling{1, 1};

		Ref<FlipBook> FlipBookAsset;

		virtual void Begin() override;
		virtual void End() override;
		virtual void Update(float dt) override;
		virtual void Render() override;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(Component)

	private:
		FlipBook *Instance = nullptr;
	};

	REFLECT_EXTERN(FlipBookComponent)

} // namespace BHive