#pragma once

#include "core/Core.h"
#include "Math.h"
#include "Plane.h"

namespace BHive
{
	struct BHIVE_API MathFunctionLibrary
	{
		template <typename T>
		static T Mix(const T &t0, const T &t1, float w);

		template <typename T>
		static T Lerp(const T &v0, const T &v1, float t);

		template <typename T>
		static T Remap(const T &v, const T &InMin, const T &InMax, const T &OutMin, const T &OutMax);

		template <typename T>
		static T Normalize(const T &v, const T &min, const T &max);

		static glm::vec3 NormalizeDeviceCoordinates(float mouse_x, float mouse_y, float width, float height);

		static glm::vec4 NdcToEyeCoordinates(const glm::vec3 &NdcCoordinates, const glm::mat4 &ProjectionMatrix);

		static glm::vec3 EyeToWorldCoordinates(const glm::vec4 &EyeCoordinates, const glm::mat4 &ViewMatrix);

		static glm::vec3 GetMouseRay(float mouse_x, float mouse_y, float width, float height, const glm::mat4 &ProjectionMatrix, const glm::mat4 &ViewMatrix);

		// construct a plane
		static FPlane CreatePlane(const glm::vec3 &p1, const glm::vec3 &normal);

		static float GetSignedDistanceToPlane(const glm::vec3 &normal, float distance, const glm::vec3 &point);
	};
} // namespace BHive

#include "MathFunctionLibrary.inl"