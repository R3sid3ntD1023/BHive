#pragma once

#include "Component.h"
#include <scene/SceneCamera.h>

struct CameraComponent : public Component
{
	CameraComponent() { SetTickEnabled(false); }
	/* data */
	BHive::SceneCamera mCamera;

	REFLECTABLEV(Component)
};
