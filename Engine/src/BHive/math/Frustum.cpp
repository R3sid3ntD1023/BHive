#include "Frustum.h"

namespace BHive
{
	Frustum::Frustum(const glm::mat4 &projection, const glm::mat4 &view)
	{

		update(projection, view);
	}

	void Frustum::update(const glm::mat4 &projection, const glm::mat4 &view)
	{
		constexpr glm::vec4 cube[8] = {
			{-1, -1, -1, 1}, {1, -1, -1, 1}, {1, 1, -1, 1}, {-1, 1, -1, 1},

			{-1, -1, 1, 1},	 {1, -1, 1, 1},	 {1, 1, 1, 1},	{-1, 1, 1, 1},
		};

		const auto inv = glm::inverse(projection * view);

		_points[0] = inv * cube[0];
		_points[1] = inv * cube[1];
		_points[2] = inv * cube[2];
		_points[3] = inv * cube[3];
		_points[4] = inv * cube[4];
		_points[5] = inv * cube[5];
		_points[6] = inv * cube[6];
		_points[7] = inv * cube[7];

		_points[0] /= _points[0].w;
		_points[1] /= _points[1].w;
		_points[2] /= _points[2].w;
		_points[3] /= _points[3].w;
		_points[4] /= _points[4].w;
		_points[5] /= _points[5].w;
		_points[6] /= _points[6].w;
		_points[7] /= _points[7].w;

		glm::vec3 center = {0, 0, 0};
		for (const auto &v : _points)
			center += glm::vec3(v);

		_position = center / 8.0f;

		_planes[0] = {_points[0], _points[1], _points[2], _points[3]};
		_planes[1] = {_points[4], _points[5], _points[6], _points[7]};
		_planes[2] = {_points[1], _points[5], _points[6], _points[3]};
		_planes[3] = {_points[0], _points[4], _points[7], _points[2]};
		_planes[4] = {_points[2], _points[3], _points[6], _points[7]};
		_planes[5] = {_points[0], _points[1], _points[5], _points[4]};
	}

	void FrustumPlane::CalculateNormal()
	{
		glm::vec3 x = Points[0] - Points[2] + Points[1] - Points[3];
		glm::vec3 y = Points[1] - Points[1] + Points[2] - Points[3];
		glm::vec3 z = glm::cross(x, y);
		Normal = (x + y + z) / 3.f;
	}
} // namespace BHive