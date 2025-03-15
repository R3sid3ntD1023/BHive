#pragma once

#include "gfx/Camera.h"

namespace BHive
{
	enum class EProjectionType
	{
		Perspective,
		Orthographic
	};

	class SceneCamera : public Camera
	{
	public:
		struct FPerspectiveSettings
		{
			float Fov = 45.0f;
			float Near = 0.01f;
			float Far = 1000.0f;
		};

		struct FOrthographicSettings
		{
			float Left = -10.0f;
			float Right = 10.f;
			float Bottom = -10.f;
			float Top = 10.0f;
			float Near = -1.0f;
			float Far = 1.0f;
		};

	public:
		SceneCamera();

		void SetProjectionType(const EProjectionType &projection);
		const EProjectionType &GetProjectionType() const { return mProjectionType; }

		void SetPerspective(float fov, float aspect, float nearclip, float farclip);
		void SetOrthographic(float size, float aspect, float nearclip, float farclip);
		void SetOrthographic(float l, float r, float b, float t, float aspect, float nearclip, float farclip);

		void SetPerspectiveSettings(const FPerspectiveSettings &settings);
		const FPerspectiveSettings &GetPerspectiveSettings() const { return mPerspectiveSettings; }

		void SetOrthographicSettings(const FOrthographicSettings &settings);
		const FOrthographicSettings &GetOrthographicSettings() const { return mOrthographicSettings; }

		void Resize(uint32_t width, uint32_t height);

	private:
		void RecalculateProjection();

	private:
		EProjectionType mProjectionType = EProjectionType::Perspective;
		FPerspectiveSettings mPerspectiveSettings;
		FOrthographicSettings mOrthographicSettings;
		float mAspectRatio = 1.7555f;
	};

} // namespace BHive