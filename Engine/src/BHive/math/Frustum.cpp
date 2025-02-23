#include "Frustum.h"

namespace BHive
{
	Frustum::Frustum(const glm::mat4 &projection, const glm::mat4 &view)
	{
		if (mProjection != projection || mView != view)
		{
			update(projection, view);
		}
	}

	void Frustum::update(const glm::mat4 &projection, const glm::mat4 &view)
	{

		const auto front = glm::dvec3(glm::normalize(view[2]));
		const auto right = glm::dvec3(glm::normalize(view[0]));
		const auto up = glm::dvec3(glm::normalize(view[1]));

		const double fov = 2.0 * atan(1.0 / projection[1][1]) /** 180.0 / PI*/;
		const double near = projection[3][2] / (projection[2][2] - 1.0);
		const double far = projection[3][2] / (projection[2][2] + 1.0);
		const double halfVSide = far * tan(fov * .5);
		const double aspect = projection[1][1] / projection[0][0];
		const double halfHSide = halfVSide * aspect;
		const glm::dvec3 frontMultFar = far * front;
		const glm::dvec3 pos = view[3];

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

		/*_planes[0] = {_points[0], _points[1], _points[2], _points[3]};
		_planes[1] = {_points[4], _points[5], _points[6], _points[7]};
		_planes[2] = {_points[1], _points[5], _points[6], _points[3]};
		_planes[3] = {_points[0], _points[4], _points[7], _points[2]};
		_planes[4] = {_points[2], _points[3], _points[6], _points[7]};
		_planes[5] = {_points[0], _points[1], _points[5], _points[4]};*/

		_planes[0] = {pos + near * front, front};							  // near
		_planes[1] = {pos + frontMultFar, -front};							  // far
		_planes[2] = {pos, glm::cross(frontMultFar - right * halfHSide, up)}; // right
		_planes[3] = {pos, glm::cross(up, frontMultFar + right * halfHSide)}; // left
		_planes[4] = {pos, glm::cross(right, frontMultFar - up * halfVSide)}; // top
		_planes[5] = {pos, glm::cross(frontMultFar + up * halfVSide, right)}; // bottom

		mProjection = projection;
		mView = view;
	}

} // namespace BHive