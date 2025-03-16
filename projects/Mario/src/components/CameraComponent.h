#pragma once

#include "Component.h"
#include "gfx/cameras/SceneCamera.h"

namespace BHive
{
	struct CameraComponent : public Component
	{
		SceneCamera Camera;
		bool Primary = true;

		void Update(float) override;
	};

} // namespace BHive