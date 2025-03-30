#pragma once

#include "Component.h"
#include "sprite/FlipBook.h"

namespace BHive
{
	REFLECT_CLASS(BHive::ClassMetaData_ComponentSpawnable)
	struct FlipBookComponent : public Component
	{
		REFLECT_CONSTRUCTOR()
		FlipBookComponent() = default;

		FlipBookComponent(const FlipBookComponent &) = default;

		REFLECT_PROPERTY()
		bool AutoPlay{true};

		REFLECT_PROPERTY()
		FColor Color{0xffffffff};

		REFLECT_PROPERTY()
		glm::vec2 Size{1, 1};

		REFLECT_PROPERTY()
		glm::vec2 Tiling{1, 1};

		REFLECT_PROPERTY()
		Ref<FlipBook> FlipBookAsset;

		virtual void Begin() override;
		virtual void End() override;
		virtual void Update(float dt) override;
		virtual void Render() override;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLE_CLASS(Component)

	private:
		FlipBook *Instance = nullptr;
	};

} // namespace BHive