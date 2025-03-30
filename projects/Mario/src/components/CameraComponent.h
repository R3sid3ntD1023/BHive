#pragma once

#include "Component.h"
#include "gfx/cameras/SceneCamera.h"

namespace BHive
{
	REFLECT_CLASS(BHive::ClassMetaData_ComponentSpawnable)
	struct CameraComponent : public Component
	{
		REFLECT_CONSTRUCTOR()
		CameraComponent() = default;
		CameraComponent(const CameraComponent &) = default;

		REFLECT_PROPERTY()
		SceneCamera Camera;

		REFLECT_PROPERTY()
		bool IsPrimary = true;

		void Update(float) override;
		void Render() override;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLE_CLASS(Component)
	};

} // namespace BHive