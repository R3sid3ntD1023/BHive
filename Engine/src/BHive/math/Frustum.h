#pragma once

#include "math/Math.h"
#include "Plane.h"

namespace BHive
{
	struct CameraData
	{
		glm::vec3 pos{}, front{}, right{}, up{};
	};

	struct Frustum
	{
		Frustum() = default;
		Frustum(const glm::mat4 &projection, const glm::mat4 &view);
		Frustum(const glm::mat4 &view, float aspect, float fov, float near, float far);

		const std::array<FPlane, 6> &_get_planes() const { return _planes; }
		const std::array<glm::vec4, 8> &get_points() const { return _points; }
		const glm::vec3 &get_position() const { return _position; }

	private:
		void update(const glm::mat4 &projection, const glm::mat4 &view);

	private:
		std::array<FPlane, 6> _planes = {};
		std::array<glm::vec4, 8> _points = {};
		glm::vec3 _position = {0, 0, 0};
	};

} // namespace BHive