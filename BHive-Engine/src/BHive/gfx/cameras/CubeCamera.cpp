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
		CalculateViews({0.f, 0.f, 0.f});
	}

	CubeCamera::CubeCamera(float nearClip, float farClip)
		: Camera(glm::perspective(glm::radians(90.0f), 1.f, nearClip, farClip)),
		  mNear(nearClip),
		  mFar(farClip)
	{
		CalculateViews({0.f, 0.f, 0.f});
	}

	const glm::mat4 &CubeCamera::GetView() const
	{
		return mViews[mCurrentFace];
	}

	glm::mat4 CubeCamera::GetViewProjection() const
	{
		return GetProjection() * GetView();
	}

	void CubeCamera::SetEye(const glm::vec3 &eye)
	{
		CalculateViews(eye);
	}

	void CubeCamera::CalculateViews(const glm::vec3 &eye)
	{
		for (uint32_t f = 0; f < 6; f++)
		{
			mViews[f] = glm::lookAt(eye, sCameraDirections[f].Target, sCameraDirections[f].Up);
		}
	}
} // namespace BHive