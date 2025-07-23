#pragma once

#include "gfx/Camera.h"

namespace BHive
{
	class BHIVE_API CubeCamera : public Camera
	{
	public:
		CubeCamera();
		CubeCamera(float nearClip, float farClip);

		const glm::mat4 &GetView(const glm::vec3 &eye, uint32_t i) const;

		const glm::mat4 &GetViewProjection(uint32_t i) const;

		virtual void Resize(uint32_t w, uint32_t h) {};

	private:
		float mNear{1.0f}, mFar{100.f};
	};
} // namespace BHive