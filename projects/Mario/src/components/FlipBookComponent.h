#pragma once

#include "sprite/FlipBook.h"
#include "Component.h"

namespace BHive
{
	DECLARE_CLASS(ClassMetaData_ComponentSpawnable)
	struct FlipBookComponent : public Component
	{
		FlipBookComponent() = default;
		FlipBookComponent(const FlipBookComponent &) = default;

		DECLARE_PROPERTY()
		bool AutoPlay = true;

		
		DECLARE_PROPERTY()
		FColor Color = 0xffffffff;

		
		DECLARE_PROPERTY()
		glm::vec2 Size = glm::vec2(1, 1);
	
		DECLARE_PROPERTY()
		glm::vec2 Tiling = glm::vec2(1, 1);

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