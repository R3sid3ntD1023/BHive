#pragma once

#include "World/Component.h"
#include "gfx/cameras/SceneCamera.h"

namespace BHive
{
	DECLARE_CLASS(BHive::ClassMetaData_ComponentSpawnable)
	struct CameraComponent : public Component, public ITickable
	{
		DECLARE_CONSTRUCTOR()
		CameraComponent() = default;
		CameraComponent(const CameraComponent &) = default;

		DECLARE_PROPERTY()
		SceneCamera Camera;

		DECLARE_PROPERTY()
		bool IsPrimary = true;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLE_CLASS(Component, ITickable)
	};

	REFLECT_EXTERN(CameraComponent)

} // namespace BHive