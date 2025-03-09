#include "PerspectiveCamera.h"

namespace BHive
{
	PerspectiveCamera::PerspectiveCamera()
	{
		mProjection = glm::perspective(glm::radians(mFov), mAspect, mNear, mFar);
	}

	PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float nearClip, float farClip)
		: mFov(fov),
		  mAspect(aspect),
		  mNear(nearClip),
		  mFar(farClip)
	{
		mProjection = glm::perspective(glm::radians(mFov), mAspect, mNear, mFar);
	}

	void PerspectiveCamera::Resize(uint32_t w, uint32_t h)
	{
		if (w == 0 || h == 0)
			return;

		mAspect = w / (float)h;
		mProjection = glm::perspective(glm::radians(mFov), mAspect, mNear, mFar);
	}
} // namespace BHive