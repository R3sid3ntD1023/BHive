#pragma once

#include "gfx/Camera.h"

namespace BHive
{
	class PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera();
		PerspectiveCamera(float fov, float aspect, float nearClip = 0.01f, float farClip = 1000.0f);

		virtual void Resize(uint32_t w, uint32_t h);

	protected:
		float mFov{45.f}, mAspect{1.3333f}, mNear{0.01f}, mFar{1000.f};
	};
} // namespace BHive