#include "MathFunctionLibrary.h"

namespace BHive
{
	glm::vec3 MathFunctionLibrary::normalize_device_coordinates(float x, float y, float width, float height)
	{
		float _x = (2.0f * x) / width - 1.0f;
		float _y = 1.0f - (2.0f * y) / height;
		float _z = 1.0f;

		return {_x, _y, _z};
	}

	glm::vec4
	MathFunctionLibrary::ndc_to_eye_coordinates(const glm::vec3 &ndc_coordinates, const glm::mat4 &projection_matrix)
	{
		glm::vec4 ray_clip = glm::vec4(ndc_coordinates.x, ndc_coordinates.y, -1.0f, 1.0f);
		glm::vec4 ray_eye = glm::inverse(projection_matrix) * ray_clip;
		return glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);
	}

	glm::vec3 MathFunctionLibrary::eye_to_world_coordinates(const glm::vec4 &eye_coordinates, const glm::mat4 &view_matrix)
	{
		glm::vec3 ray_world = (glm::inverse(view_matrix) * eye_coordinates);

		return glm::normalize(ray_world);
	}

	glm::vec3 MathFunctionLibrary::get_mouse_ray(
		float x, float y, float width, float height, const glm::mat4 &projection_matrix, const glm::mat4 &view_matrix)
	{
		auto normlized_coords = normalize_device_coordinates(x, y, width, height);
		auto eyecoords = ndc_to_eye_coordinates(normlized_coords, projection_matrix);
		auto world_coords = eye_to_world_coordinates(eyecoords, view_matrix);
		return world_coords;
	}

	FPlane MathFunctionLibrary::create_plane(const glm::vec3 &p1, const glm::vec3 &normal)
	{
		FPlane p;
		p.Normal = glm::normalize(normal);
		p.Distance = glm::dot(p.Normal, p1);

		return p;
	}

	float MathFunctionLibrary::get_signed_distance_to_plane(const FPlane &plane, const glm::vec3 &point)
	{
		return glm::dot(plane.Normal, point) - plane.Distance;
	}

	template <>
	FTransform MathFunctionLibrary::lerp(const FTransform &v0, const FTransform &v1, float t)
	{
		auto translation = lerp(v0.get_translation(), v1.get_translation(), t);
		auto rotation = lerp(v0.get_rotation(), v1.get_rotation(), t);
		auto scale = lerp(v0.get_scale(), v1.get_scale(), t);

		return FTransform{translation, rotation, scale};
	}

} // namespace BHive