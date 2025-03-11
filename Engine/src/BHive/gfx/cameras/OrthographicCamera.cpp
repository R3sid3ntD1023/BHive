#include "OrthographicCamera.h"

namespace BHive
{
	OrthographicCamera::OrthographicCamera()
		: Camera(glm::ortho(mLeft * mAspect, mRight * mAspect, mBottom, mTop, mNear, mFar))
	{
	}

	OrthographicCamera::OrthographicCamera(
		float left, float right, float bottom, float top, float aspect, float nearClip, float farClip)
		: Camera(glm::ortho(left * aspect, right * aspect, bottom, top, nearClip, farClip)),
		  mLeft(left),
		  mRight(right),
		  mBottom(bottom),
		  mTop(top),
		  mNear(nearClip),
		  mFar(farClip),
		  mAspect(aspect)
	{
	}

	OrthographicCamera::OrthographicCamera(float size, float aspect, float nearClip, float farClip)
		: Camera(glm::ortho(-size * .5f * aspect, size * .5f * aspect, -size * .5f, size * .5f, nearClip, farClip)),
		  mLeft(-size * .5f),
		  mRight(size * .5f),
		  mBottom(-size * .5f),
		  mTop(size * .5f),
		  mNear(nearClip),
		  mFar(farClip),
		  mAspect(aspect)
	{
	}

	void OrthographicCamera::Resize(uint32_t w, uint32_t h)
	{
		mAspect = w / (float)h;
		mProjection = glm::ortho(mLeft * mAspect, mRight * mAspect, mBottom, mTop, mNear, mFar);
	}
} // namespace BHive