#pragma once

#include "gfx/Camera.h"
#include "serialization/Serialization.h"
#include "reflection/Reflection.h"

namespace BHive
{
	enum class EProjectionType
	{
		Perspective,
		Orthographic
	};

	class BHIVE SceneCamera : public Camera, public ISerializable
	{
	public:
		struct FPerspectiveSettings
		{
			float mFov = 45.0f;
			float mNear = 0.01f;
			float mFar = 1000.0f;

		
		};

		struct FOrthographicSettings
		{
			float mSize = 10.0f;
			float mNear = -1.0f;
			float mFar = 1.0f;

		};

	public:
		SceneCamera();

		void SetProjectionType(const EProjectionType &projection);
		const EProjectionType &GetProjectionType() const { return mProjectionType; }

		void SetPerspective(float fov, float nearclip, float farclip);
		void SetOrthographic(float size, float nearclip, float farclip);

		void SetPerspectiveSettings(const FPerspectiveSettings &settings);
		const FPerspectiveSettings &GetPerspectiveSettings() const { return mPerspectiveSettings; }

		void SetOrthographicSettings(const FOrthographicSettings &settings);
		const FOrthographicSettings &GetOrthographicSettings() const { return mOrthographicSettings; }

		void Resize(uint32_t width, uint32_t height);

		void Serialize(StreamWriter& ar) const;
		void Deserialize(StreamReader& ar);

	private:
		void RecalculateProjection();

	private:
		EProjectionType mProjectionType = EProjectionType::Perspective;
		FPerspectiveSettings mPerspectiveSettings;
		FOrthographicSettings mOrthographicSettings;
		float mAspectRatio = 1.7555f;
	};

	void Serialize(StreamWriter& ar, const SceneCamera::FPerspectiveSettings& obj);
	void Deserialize(StreamReader& ar, SceneCamera::FPerspectiveSettings& obj);

	void Serialize(StreamWriter& ar, const SceneCamera::FOrthographicSettings& obj);
	void Deserialize(StreamReader& ar, SceneCamera::FOrthographicSettings& obj);

	REFLECT_EXTERN(EProjectionType);
	REFLECT_EXTERN(SceneCamera::FPerspectiveSettings);
	REFLECT_EXTERN(SceneCamera::FOrthographicSettings);
	REFLECT_EXTERN(SceneCamera);

}