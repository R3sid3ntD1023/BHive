#pragma once

#include "gfx/cameras/SceneCamera.h"

namespace BHive
{
	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true;
	};

} // namespace BHive