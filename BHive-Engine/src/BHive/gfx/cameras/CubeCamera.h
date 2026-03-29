#pragma once

#include "gfx/Camera.h"

namespace BHive
{
	class BHIVE_API CubeCamera : public Camera
	{
	public:
		CubeCamera();

		CubeCamera(float nearClip, float farClip);

		const glm::mat4& GetView() const;

		glm::mat4 GetViewProjection() const;

		virtual void Resize(uint32_t w, uint32_t h) {};

		void SetFace(uint32_t face) { mCurrentFace = face; }

		void SetEye(const glm::vec3 &eye);

	private:
		void CalculateViews(const glm::vec3& eye);

	private:
		float mNear{1.0f}, mFar{100.f};
		uint32_t mCurrentFace = 0;
		glm::mat4 mViews[6];
	};
} // namespace BHive