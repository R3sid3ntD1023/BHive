#include "SceneCamera.h"
#include "math/Math.h"

namespace BHive
{
	SceneCamera::SceneCamera()
	{
		RecalculateProjection();
	}

	void SceneCamera::SetProjectionType(const EProjectionType &projection)
	{
		mProjectionType = projection;
		RecalculateProjection();
	}

	void SceneCamera::SetPerspective(float fov, float aspect, float nearclip, float farclip)
	{
		mProjectionType = EProjectionType::Perspective;
		mPerspectiveSettings = FPerspectiveSettings(fov, nearclip, farclip);
		mAspectRatio = aspect;
		RecalculateProjection();
	}

	void SceneCamera::SetOrthographic(float size, float aspect, float nearclip, float farclip)
	{
		mProjectionType = EProjectionType::Orthographic;
		mOrthographicSettings = FOrthographicSettings(-size * .5f, size * .5f, -size * .5f, size * .5f, nearclip, farclip);
		mAspectRatio = aspect;
		RecalculateProjection();
	}

	void SceneCamera::SetOrthographic(float l, float r, float b, float t, float aspect, float nearclip, float farclip)
	{
		mProjectionType = EProjectionType::Orthographic;
		mOrthographicSettings = FOrthographicSettings(l, r, b, t, nearclip, farclip);
		mAspectRatio = aspect;
		RecalculateProjection();
	}

	void SceneCamera::SetPerspectiveSettings(const FPerspectiveSettings &settings)
	{
		mPerspectiveSettings = settings;
		RecalculateProjection();
	}

	void SceneCamera::SetOrthographicSettings(const FOrthographicSettings &settings)
	{
		mOrthographicSettings = settings;
		RecalculateProjection();
	}

	void SceneCamera::Resize(uint32_t width, uint32_t height)
	{
		mAspectRatio = width / (float)height;
		RecalculateProjection();
	}

	void SceneCamera::RecalculateProjection()
	{
		switch (mProjectionType)
		{
		case BHive::EProjectionType::Perspective:
		{
			auto &settings = mPerspectiveSettings;
			mProjection = glm::perspective(glm::radians(settings.Fov), mAspectRatio, settings.Near, settings.Far);
		}
		break;
		case BHive::EProjectionType::Orthographic:
		{
			auto &settings = mOrthographicSettings;
			mProjection = glm::ortho(
				settings.Left * mAspectRatio, settings.Right * mAspectRatio, settings.Bottom, settings.Top, settings.Near,
				settings.Far);
		}
		break;
		default:
			break;
		}
	}

} // namespace BHive