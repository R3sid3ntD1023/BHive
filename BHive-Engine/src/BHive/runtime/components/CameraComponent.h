#pragma once

#include "runtime/Component.h"
#include "gfx/cameras/SceneCamera.h"

namespace BHive
{

	struct BHIVE_API CameraComponent : public Component, public ITickable
	{
		CameraComponent() = default;
		CameraComponent(const CameraComponent &) = default;

		SceneCamera Camera;

		bool IsPrimary = true;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLE_CLASS(Component, ITickable)
	};

	REFLECT_EXTERN(CameraComponent)

} // namespace BHive