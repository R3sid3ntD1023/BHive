#pragma once

#include "gfx/Camera.h"

namespace BHive
{
	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera();

		OrthographicCamera(
			float left, float right, float bottom, float top, float aspect, float nearClip = 0, float farClip = 1);
		OrthographicCamera(float size, float aspect, float nearClip = 0, float farClip = 1);

		virtual void Resize(uint32_t w, uint32_t h);

	private:
		float mLeft{-1}, mRight{1}, mBottom{-1}, mTop{1};
		float mNear{0}, mFar{1};
		float mAspect{1.3333f};
	};
} // namespace BHive