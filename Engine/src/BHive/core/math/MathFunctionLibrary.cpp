#include "MathFunctionLibrary.h"

namespace BHive
{
	glm::vec3 MathFunctionLibrary::NormalizeDeviceCoordinates(float x, float y, float width, float height)
	{
		float X = (2.0f * x) / width - 1.0f;
		float Y = 1.0f - (2.0f * y) / height;
		float Z = 1.0f;

		return {X, Y, Z};
	}

	glm::vec4 MathFunctionLibrary::NdcToEyeCoordinates(const glm::vec3 &NdcCoordinates, const glm::mat4 &ProjectionMatrix)
	{
		glm::vec4 RayClip = glm::vec4(NdcCoordinates.x, NdcCoordinates.y, -1.0f, 1.0f);
		glm::vec4 RayEye = glm::inverse(ProjectionMatrix) * RayClip;
		return glm::vec4(RayEye.x, RayEye.y, -1.0f, 0.0f);
	}

	glm::vec3 MathFunctionLibrary::EyeToWorldCoordinates(const glm::vec4 &EyeCoordinates, const glm::mat4 &ViewMatrix)
	{
		glm::vec3 RayWorld = (glm::inverse(ViewMatrix) * EyeCoordinates);

		return glm::normalize(RayWorld);
	}

	glm::vec3 MathFunctionLibrary::GetMouseRay(float x, float y, float width, float height, const glm::mat4 &ProjectionMatrix, const glm::mat4 &ViewMatrix)
	{
		auto NormlizedCoords = NormalizeDeviceCoordinates(x, y, width, height);
		auto eyecoords = NdcToEyeCoordinates(NormlizedCoords, ProjectionMatrix);
		auto WorldCoords = EyeToWorldCoordinates(eyecoords, ViewMatrix);
		return WorldCoords;
	}

	FPlane MathFunctionLibrary::CreatePlane(const glm::vec3 &p1, const glm::vec3 &normal)
	{
		FPlane p;
		p.Normal = glm::normalize(normal);
		p.Distance = glm::dot(p.Normal, p1);

		return p;
	}

	float MathFunctionLibrary::GetSignedDistanceToPlane(const FPlane &plane, const glm::vec3 &point)
	{
		return glm::dot(plane.Normal, point) - plane.Distance;
	}

	template <>
	FTransform MathFunctionLibrary::Lerp(const FTransform &v0, const FTransform &v1, float t)
	{
		auto translation = Lerp(v0.GetTranslation(), v1.GetTranslation(), t);
		auto rotation = Lerp(v0.GetRotation(), v1.GetRotation(), t);
		auto scale = Lerp(v0.GetScale(), v1.GetScale(), t);

		return FTransform{translation, rotation, scale};
	}

} // namespace BHive