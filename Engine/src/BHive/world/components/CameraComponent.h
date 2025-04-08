#pragma once

#include "Component.h"
#include "gfx/cameras/SceneCamera.h"

namespace BHive
{
	DECLARE_CLASS(BHive::ClassMetaData_ComponentSpawnable)
	struct CameraComponent : public Component
	{
		DECLARE_CONSTRUCTOR()
		CameraComponent() = default;
		CameraComponent(const CameraComponent &) = default;

		DECLARE_PROPERTY()
		SceneCamera Camera;

		DECLARE_PROPERTY()
		bool IsPrimary = true;

		void Update(float) override;
		void Render() override;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLE_CLASS(Component)
	};

	REFLECT_EXTERN(CameraComponent)

} // namespace BHive