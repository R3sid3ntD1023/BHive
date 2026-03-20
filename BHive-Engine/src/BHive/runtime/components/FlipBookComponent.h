#pragma once

#include "runtime/Component.h"
#include "gfx/sprite/FlipBook.h"

namespace BHive
{

	struct BHIVE_API FlipBookComponent : public Component, public ITickable
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

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		FlipBook *Instance() const { return mInstance; }

		REFLECTABLE_CLASS(Component, ITickable)

	private:
		FlipBook *mInstance = nullptr;
	};

	REFLECT_EXTERN(FlipBookComponent)

} // namespace BHive