#pragma once

#include "Component.h"
#include "gfx/cameras/SceneCamera.h"

namespace BHive
{
	struct CameraComponent : public Component
	{
		CameraComponent() = default;
		CameraComponent(const CameraComponent &) = default;

		SceneCamera Camera;
		bool IsPrimary = true;

		void Update(float) override;
		void Render() override;

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;
		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(Component)
	};

	REFLECT_EXTERN(CameraComponent)

} // namespace BHive