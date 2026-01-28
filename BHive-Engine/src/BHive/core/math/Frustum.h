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

		const std::array<FPlane, 6> &GetPlanes() const { return mPlanes; }

		void Update(const glm::mat4 &projection, const glm::mat4 &view);
		void Update(const glm::mat4 &view, float aspect, float fov, float near, float far);

	private:
		std::array<FPlane, 6> mPlanes = {};
	};

	struct FrustumViewer
	{
		FrustumViewer(const glm::mat4 &projection, const glm::mat4 &view);

		const std::array<glm::vec4, 8> &GetPoints() const { return mPoints; }
		const glm::vec3 &GetPosition() const { return mPosition; }

	private:
		void CalculatePoints(const glm::mat4 &projection, const glm::mat4 &view);

		std::array<glm::vec4, 8> mPoints = {};
		glm::vec3 mPosition{};
	};

} // namespace BHive