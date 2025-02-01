#pragma once

#include "math/Math.h"

namespace BHive
{
	inline BHIVE glm::vec3 NormalizeDeviceCoordinates(float x, float y, float width, float height)
	{
		float _x = (2.0f * x) / width - 1.0f;
		float _y = 1.0f - (2.0f * y) / height;
		float _z = 1.0f;

		return {_x, _y, _z};
	}

	inline BHIVE glm::vec4 NDCToEyeCoordinates(const glm::vec3 &ndc_coordinates, const glm::mat4 &projection_matrix)
	{
		glm::vec4 ray_clip = glm::vec4(ndc_coordinates.x, ndc_coordinates.y, -1.0f, 1.0f);
		glm::vec4 ray_eye = glm::inverse(projection_matrix) * ray_clip;
		return glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);
	}

	inline BHIVE glm::vec3 EyeToWorldCoordinates(const glm::vec4 &eye_coordinates, const glm::mat4 &view_matrix)
	{
		glm::vec3 ray_world = (glm::inverse(view_matrix) * eye_coordinates);

		return glm::normalize(ray_world);
	}

	inline BHIVE glm::vec3 GetMouseRay(float x, float y, float width, float height, const glm::mat4 &projection_matrix, const glm::mat4 &view_matrix)
	{
		auto normlized_coords = NormalizeDeviceCoordinates(x, y, width, height);
		auto eyecoords = NDCToEyeCoordinates(normlized_coords, projection_matrix);
		auto world_coords = EyeToWorldCoordinates(eyecoords, view_matrix);
		return world_coords;
	}
} // namespace BHive