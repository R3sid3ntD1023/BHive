#pragma once

#include "math/Math.h"
#include "Plane.h"

namespace BHive
{

	struct Frustum
	{
		Frustum() = default;
		Frustum(const glm::mat4 &projection, const glm::mat4 &view);

		void update(const glm::mat4 &projection, const glm::mat4 &view);

		const std::array<FPlane, 6> &_get_planes() const { return _planes; }
		const std::array<glm::vec4, 8> &get_points() const { return _points; }
		const glm::vec3 &get_position() const { return _position; }

	private:
		std::array<FPlane, 6> _planes;
		std::array<glm::vec4, 8> _points;
		glm::vec3 _position = {0, 0, 0};
	};

} // namespace BHive