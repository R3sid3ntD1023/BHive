#include "Frustum.h"
#include "MathFunctionLibrary.h"

#ifdef near
	#undef near
#endif

#ifdef far
	#undef far
#endif

namespace BHive
{
	Frustum::Frustum(const glm::mat4 &projection, const glm::mat4 &view)
	{
		Update(projection, view);
	}

	Frustum::Frustum(const glm::mat4 &view, float aspect, float fov, float near, float far)
	{
		Update(glm::perspective(fov, aspect, near, far), view);
	}

	void Frustum::Update(const glm::mat4 &projection, const glm::mat4 &view)
	{
		const auto view_inv = glm::inverse(projection * view);

		constexpr glm::vec4 cube[8] = {
			{-1, -1, -1, 1}, {1, -1, -1, 1}, {1, 1, -1, 1}, {-1, 1, -1, 1},

			{-1, -1, 1, 1},	 {1, -1, 1, 1},	 {1, 1, 1, 1},	{-1, 1, 1, 1},
		};

		for (uint32_t i = 0; i < 8; i++)
		{
			glm::vec4 pos = view_inv * cube[i];
			pos /= pos.w;
			mPoints[i] = glm::vec3(pos);
		}

		auto MakePlane = [](glm::vec3 a, glm::vec3 b, glm::vec3 c)
		{
			glm::vec3 n = glm::normalize(glm::cross(b - a, c - a));
			float d = -glm::dot(n, a);
			return glm::vec4{n, d};
		};

		mPlanes[0] = MakePlane(mPoints[0], mPoints[1], mPoints[2]); // near
		mPlanes[1] = MakePlane(mPoints[5], mPoints[4], mPoints[7]); // far
		mPlanes[2] = MakePlane(mPoints[4], mPoints[0], mPoints[3]); // left
		mPlanes[3] = MakePlane(mPoints[1], mPoints[5], mPoints[6]); // right
		mPlanes[4] = MakePlane(mPoints[3], mPoints[2], mPoints[6]); // top
		mPlanes[5] = MakePlane(mPoints[4], mPoints[5], mPoints[1]); // bottom
	}

} // namespace BHive