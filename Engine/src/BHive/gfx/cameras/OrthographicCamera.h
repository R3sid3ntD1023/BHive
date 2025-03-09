#pragma once

#include "gfx/Camera.h"

namespace BHive
{
	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera();

		OrthographicCamera(
			float left, float right, float bottom, float top, float nearClip = 0, float farClip = 1,
			bool fixedAspect = true);
		OrthographicCamera(float size, float nearClip = 0, float farClip = 1, bool fixedAspect = true);

		virtual void Resize(uint32_t w, uint32_t h);

	private:
		float mLeft{-1}, mRight{1}, mBottom{-1}, mTop{1};
		float mNear{0}, mFar{1};
		bool mFixedAspect{true};
	};
} // namespace BHive