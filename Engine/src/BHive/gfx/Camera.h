#pragma once

#include "core/Core.h"

namespace BHive
{
	class BHIVE_API Camera
	{
	public:
		Camera() = default;
		Camera(const glm::mat4 &projection)
			: mProjection(projection)
		{
		}

		const glm::mat4 &GetProjection() const { return mProjection; }

		virtual void Resize(uint32_t w, uint32_t h) = 0;

	protected:
		glm::mat4 mProjection;
	};
} // namespace BHive