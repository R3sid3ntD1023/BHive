#include "OrthographicCamera.h"

namespace BHive
{
	OrthographicCamera::OrthographicCamera()
		: Camera(glm::ortho(mLeft, mRight, mBottom, mTop, mNear, mFar))
	{
	}

	OrthographicCamera::OrthographicCamera(
		float left, float right, float bottom, float top, float nearClip, float farClip, bool fixedAspect)
		: Camera(glm::ortho(left, right, bottom, top, nearClip, farClip)),
		  mLeft(left),
		  mRight(right),
		  mBottom(bottom),
		  mTop(top),
		  mNear(nearClip),
		  mFar(farClip),
		  mFixedAspect(fixedAspect)
	{
	}

	OrthographicCamera::OrthographicCamera(float size, float nearClip, float farClip, bool fixedAspect)
		: Camera(glm::ortho(-size * .5f, size * .5f, -size * .5f, size * .5f, nearClip, farClip)),
		  mLeft(-size * .5f),
		  mRight(size * .5f),
		  mBottom(-size * .5f),
		  mTop(size * .5f),
		  mNear(nearClip),
		  mFar(farClip),
		  mFixedAspect(fixedAspect)
	{
	}

	void OrthographicCamera::Resize(uint32_t w, uint32_t h)
	{
		if (mFixedAspect)
		{
			float aspect = w / (float)h;
			mProjection = glm::ortho(mLeft, mRight * aspect, mBottom * aspect, mTop, mNear, mFar);
		}
	}
} // namespace BHive