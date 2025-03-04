#pragma once

#include "Component.h"
#include <scene/SceneCamera.h>

BEGIN_NAMESPACE(BHive)

struct CameraComponent : public Component
{
	CameraComponent() { SetTickEnabled(false); }
	/* data */
	BHive::SceneCamera mCamera;

	REFLECTABLEV(Component)
};

END_NAMESPACE
