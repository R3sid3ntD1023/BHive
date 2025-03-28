#pragma once

#include "Component.h"
#include "sprite/FlipBook.h"

namespace BHive
{
	DECLARE_CLASS(BHive::ClassMetaData_ComponentSpawnable)
	struct FlipBookComponent : public Component
	{
		DECLARE_CONSTRUCTOR()
		FlipBookComponent() = default;

		FlipBookComponent(const FlipBookComponent &) = default;

		DECLARE_PROPERTY()
		bool AutoPlay{true};

		DECLARE_PROPERTY()
		FColor Color{0xffffffff};

		DECLARE_PROPERTY()
		glm::vec2 Size{1, 1};

		DECLARE_PROPERTY()
		glm::vec2 Tiling{1, 1};

		DECLARE_PROPERTY()
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

} // namespace BHive