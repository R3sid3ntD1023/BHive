#pragma once

#include "Math.h"
#include "Plane.h"

namespace BHive
{
	struct BHIVE_API Frustum
	{
		Frustum() = default;
		Frustum(const glm::mat4 &projection, const glm::mat4 &view);
		Frustum(const glm::mat4 &view, float aspect, float fov, float near, float far);

		void Update(const glm::mat4 &projection, const glm::mat4 &view);

		const std::array<glm::vec4, 6> &GetPlanes() const { return mPlanes; }

		const std::array<glm::vec3, 8> &GetPoints() const { return mPoints; }

	private:
		std::array<glm::vec4, 6> mPlanes;
		std::array<glm::vec3, 8> mPoints;
	};

} // namespace BHive