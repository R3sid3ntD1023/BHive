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

	void SceneCamera::SetPerspective(float fov, float nearclip, float farclip)
	{
		mProjectionType = EProjectionType::Perspective;
		mPerspectiveSettings = FPerspectiveSettings(fov, nearclip, farclip);
		RecalculateProjection();
	}

	void SceneCamera::SetOrthographic(float size, float nearclip, float farclip)
	{
		mProjectionType = EProjectionType::Orthographic;
		mOrthographicSettings = FOrthographicSettings(size, nearclip, farclip);
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

	void SceneCamera::Serialize(StreamWriter& ar) const
	{
		ar(mProjectionType, mPerspectiveSettings, mOrthographicSettings);
	}

	void SceneCamera::Deserialize(StreamReader& ar)
	{
		ar(mProjectionType, mPerspectiveSettings, mOrthographicSettings);
		RecalculateProjection();
	}

	void SceneCamera::RecalculateProjection()
	{
		switch (mProjectionType)
		{
		case BHive::EProjectionType::Perspective:
		{
			auto &settings = mPerspectiveSettings;
			mProjection = glm::perspective(glm::radians(settings.mFov), mAspectRatio, settings.mNear, settings.mFar);
		}
		break;
		case BHive::EProjectionType::Orthographic:
		{
			auto &settings = mOrthographicSettings;
			auto size = settings.mSize * 0.5f;
			mProjection = glm::ortho(-size * mAspectRatio, size * mAspectRatio, -size, size, settings.mNear, settings.mFar);
		}
		break;
		default:
			break;
		}
	}


	void Serialize(StreamWriter& ar, const SceneCamera::FPerspectiveSettings& obj)
	{
		ar(obj.mFov, obj.mNear, obj.mFar);
	}

	void Deserialize(StreamReader& ar, SceneCamera::FPerspectiveSettings& obj)
	{
		ar(obj.mFov, obj.mNear, obj.mFar);
	}

	void Serialize(StreamWriter& ar, const SceneCamera::FOrthographicSettings& obj)
	{
		ar(obj.mSize, obj.mNear, obj.mFar);
	}

	void Deserialize(StreamReader& ar, SceneCamera::FOrthographicSettings& obj)
	{
		ar(obj.mSize, obj.mNear, obj.mFar);
	}

	REFLECT(EProjectionType)
	{
		BEGIN_REFLECT_ENUM(EProjectionType)
			(
				ENUM_VALUE(Perspective),
				ENUM_VALUE(Orthographic));
	}

	REFLECT(SceneCamera::FPerspectiveSettings)
	{
		BEGIN_REFLECT(SceneCamera::FPerspectiveSettings)
			REFLECT_PROPERTY("Fov", mFov)
			REFLECT_PROPERTY("Near", mNear)
			REFLECT_PROPERTY("Far", mFar);
	}

	REFLECT(SceneCamera::FOrthographicSettings)
	{
		BEGIN_REFLECT(SceneCamera::FOrthographicSettings)
			REFLECT_PROPERTY("Size", mSize)
			REFLECT_PROPERTY("Near", mNear)
			REFLECT_PROPERTY("Far", mFar);
	}

	REFLECT(SceneCamera)
	{
		BEGIN_REFLECT(SceneCamera)
			REFLECT_PROPERTY("ProjectionType", GetProjectionType, SetProjectionType)
			REFLECT_PROPERTY("PerspectiveSettings", GetPerspectiveSettings, SetPerspectiveSettings)
			REFLECT_PROPERTY("OrthographicSettings", GetOrthographicSettings, SetOrthographicSettings);
	}

}