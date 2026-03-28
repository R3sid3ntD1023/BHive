#include "CubeCamera.h"

namespace BHive
{
	struct CameraDirection
	{
		glm::vec3 Target;
		glm::vec3 Up;
	};

	CameraDirection sCameraDirections[] = {
		{{1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
		{{-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
		{{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
		{{0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
		{{0.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
		{{0.0f, 0.0f, -1.0f}, {0.0f, -1.0f, 0.0f}}};

	CubeCamera::CubeCamera()
		: Camera(glm::perspective(glm::radians(90.0f), 1.0f, mNear, mFar))
	{
	}
	CubeCamera::CubeCamera(float nearClip, float farClip)
		: Camera(glm::perspective(glm::radians(90.0f), 1.f, nearClip, farClip)),
		  mNear(nearClip),
		  mFar(farClip)
	{
	}

	glm::mat4 CubeCamera::GetView(const glm::vec3 &eye, uint32_t i) const
	{
		return glm::lookAt(eye, sCameraDirections[i].Target, sCameraDirections[i].Up);
	}

	const glm::mat4 &CubeCamera::GetViewProjection(uint32_t i) const
	{
		return mProjection * GetView(glm::vec3(0.f), i);
	}
} // namespace BHive