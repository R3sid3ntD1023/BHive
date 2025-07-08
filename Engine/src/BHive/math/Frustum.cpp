#include "Frustum.h"

namespace BHive
{
	Frustum::Frustum(const glm::mat4 &projection, const glm::mat4 &view)
	{
		Update(projection, view);
	}

	Frustum::Frustum(const glm::mat4 &view, float aspect, float fov, float near, float far)
	{
		Update(view, aspect, fov, near, far);
	}

	void Frustum::Update(const glm::mat4 &projection, const glm::mat4 &view)
	{
		const auto view_inv = glm::inverse(view);
		const auto forward = -glm::normalize(glm::vec3(view_inv[2]));
		const auto right = glm::normalize(glm::vec3(view_inv[0]));
		const auto up = glm::normalize(glm::vec3(view_inv[1]));

		auto fov = 2.0f * atan(1.0f / projection[1][1]);
		const auto near = projection[3][2] / (projection[2][2] - 1.0f);
		const auto far = projection[3][2] / (projection[2][2] + 1.0f);
		const auto aspect = projection[1][1] / projection[0][0];

		const auto half_v = far * tanf(fov * .5f);
		const auto half_h = half_v * aspect;
		const glm::vec3 frontMultFar = far * forward;
		const glm::vec3 pos = view_inv[3].xyz;

		mPlanes[0] = CreatePlane(pos + near * forward, forward);					  // near
		mPlanes[1] = CreatePlane(pos + frontMultFar, -forward);						  // far
		mPlanes[2] = CreatePlane(pos, glm::cross(frontMultFar - right * half_h, up)); // right
		mPlanes[3] = CreatePlane(pos, glm::cross(up, frontMultFar + right * half_h)); // left
		mPlanes[4] = CreatePlane(pos, glm::cross(right, frontMultFar - up * half_v)); // top
		mPlanes[5] = CreatePlane(pos, glm::cross(frontMultFar + up * half_v, right)); // bottom
	}

	void Frustum::Update(const glm::mat4 &view, float aspect, float fov, float near, float far)
	{
		const auto view_inv = glm::inverse(view);
		const auto forward = -glm::normalize(glm::vec3(view[2]));
		const auto right = glm::normalize(glm::cross(forward, {0, 1, 0}));
		const auto up = glm::normalize(glm::cross(right, forward));
		const auto half_v = far * tanf(fov * .5f);
		const auto half_h = half_v * aspect;
		const glm::vec3 frontMultFar = far * forward;
		const glm::vec3 pos = view[3].xyz;

		mPlanes[0] = CreatePlane(pos + near * forward, forward);					  // near
		mPlanes[1] = CreatePlane(pos + frontMultFar, -forward);						  // far
		mPlanes[2] = CreatePlane(pos, glm::cross(frontMultFar - right * half_h, up)); // right
		mPlanes[3] = CreatePlane(pos, glm::cross(up, frontMultFar + right * half_h)); // left
		mPlanes[4] = CreatePlane(pos, glm::cross(right, frontMultFar - up * half_v)); // top
		mPlanes[5] = CreatePlane(pos, glm::cross(frontMultFar + up * half_v, right)); // bottom
	}

	FrustumViewer::FrustumViewer(const glm::mat4 &projection, const glm::mat4 &view)
	{
		CalculatePoints(projection, view);
	}

	void FrustumViewer::CalculatePoints(const glm::mat4 &projection, const glm::mat4 &view)
	{
		constexpr glm::vec4 cube[8] = {
			{-1, -1, -1, 1}, {1, -1, -1, 1}, {1, 1, -1, 1}, {-1, 1, -1, 1},

			{-1, -1, 1, 1},	 {1, -1, 1, 1},	 {1, 1, 1, 1},	{-1, 1, 1, 1},
		};

		const auto inv = glm::inverse(projection * view);

		mPoints[0] = inv * cube[0];
		mPoints[1] = inv * cube[1];
		mPoints[2] = inv * cube[2];
		mPoints[3] = inv * cube[3];
		mPoints[4] = inv * cube[4];
		mPoints[5] = inv * cube[5];
		mPoints[6] = inv * cube[6];
		mPoints[7] = inv * cube[7];

		mPoints[0] /= mPoints[0].w;
		mPoints[1] /= mPoints[1].w;
		mPoints[2] /= mPoints[2].w;
		mPoints[3] /= mPoints[3].w;
		mPoints[4] /= mPoints[4].w;
		mPoints[5] /= mPoints[5].w;
		mPoints[6] /= mPoints[6].w;
		mPoints[7] /= mPoints[7].w;

		mPosition =
			(mPoints[0] + mPoints[1] + mPoints[2] + mPoints[3] + mPoints[4] + mPoints[5] + mPoints[6] + mPoints[7]) / 8.f;
	}

} // namespace BHive