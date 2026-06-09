#pragma once

#include "core/Core.h"

namespace BHive
{
	struct alignas(16) FCameraData
	{
		glm::mat4 View{1};
		glm::mat4 Proj{1};
		glm::vec4 NearFar{};
		glm::vec3 CameraPos{};
	};

	class FrameGlobals
	{
	public:
		void SetCamera(const FCameraData &data)
		{
			mCamera = data;
		}

		const FCameraData &GetCamera() { return mCamera; }

		static FrameGlobals &Get()
		{
			static FrameGlobals sInstance;
			return sInstance;
		}

	private:
		FrameGlobals() = default;

	private:
		FCameraData mCamera;
	};
}