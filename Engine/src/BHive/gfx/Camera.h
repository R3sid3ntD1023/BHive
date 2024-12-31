#pragma once

#include "core/Core.h"
#include "math/Math.h"

namespace BHive
{
	
	
	class BHIVE Camera
	{
	public:

		Camera() = default;
		Camera(const glm::mat4& projection)
			:mProjection(projection)
		{}

		const glm::mat4& GetProjection() const { return mProjection; }


	protected:
		glm::mat4 mProjection;
		
	};
}