#pragma once

#include "gfx/Camera.h"

namespace BHive
{
	class CubeCamera : public Camera
	{
	public:
		CubeCamera();
		CubeCamera(float nearClip, float farClip);

		const glm::mat4 &GetView(const glm::vec3 &eye, uint32_t i);
		virtual void Resize(uint32_t w, uint32_t h) {};

	private:
		float mNear{1.f}, mFar{100.f};
	};
} // namespace BHive