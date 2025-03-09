#include "CubeCamera.h"
#include <glad/glad.h>

namespace BHive
{
	struct CameraDirection
	{
		GLenum CubemapFace;
		glm::vec3 Target;
		glm::vec3 Up;
	};

	CameraDirection sCameraDirections[] = {
		{GL_TEXTURE_CUBE_MAP_POSITIVE_X, {1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
		{GL_TEXTURE_CUBE_MAP_NEGATIVE_X, {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
		{GL_TEXTURE_CUBE_MAP_POSITIVE_Y, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
		{GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
		{GL_TEXTURE_CUBE_MAP_POSITIVE_Z, {0.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
		{GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, {0.0f, 0.0f, -1.0f}, {0.0f, -1.0f, 0.0f}}};

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

	const glm::mat4 &CubeCamera::GetView(const glm::vec3 &eye, uint32_t i)
	{
		return glm::lookAt(eye, sCameraDirections[i].Target, sCameraDirections[i].Up);
	}
} // namespace BHive