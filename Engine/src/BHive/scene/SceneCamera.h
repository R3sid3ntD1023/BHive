#pragma once

#include "gfx/Camera.h"
#include "core/reflection/Reflection.h"

namespace BHive
{
	enum class EProjectionType
	{
		Perspective,
		Orthographic
	};

	class BHIVE SceneCamera : public Camera
	{
	public:
		struct FPerspectiveSettings
		{
			float mFov = 45.0f;
			float mNear = 0.01f;
			float mFar = 1000.0f;

			template <typename A>
			inline void Serialize(A &ar)
			{
				ar(mFov, mNear, mFar);
			}
		};

		struct FOrthographicSettings
		{
			float mSize = 10.0f;
			float mNear = -1.0f;
			float mFar = 1.0f;

			template <typename A>
			inline void Serialize(A &ar)
			{
				ar(mSize, mNear, mFar);
			}
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

		template <typename A>
		inline void Save(A &ar) const
		{
			ar(mProjectionType, mPerspectiveSettings, mOrthographicSettings);
		}

		template <typename A>
		inline void Load(A &ar)
		{
			ar(mProjectionType, mPerspectiveSettings, mOrthographicSettings);

			RecalculateProjection();
		}

	private:
		void RecalculateProjection();

	private:
		EProjectionType mProjectionType = EProjectionType::Perspective;
		FPerspectiveSettings mPerspectiveSettings;
		FOrthographicSettings mOrthographicSettings;
		float mAspectRatio = 1.7555f;
	};

	REFLECT_EXTERN(EProjectionType);
	REFLECT_EXTERN(SceneCamera::FPerspectiveSettings);
	REFLECT_EXTERN(SceneCamera::FOrthographicSettings);
	REFLECT_EXTERN(SceneCamera);

} // namespace BHive