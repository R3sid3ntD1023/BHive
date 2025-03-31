#pragma once

#include "core/reflection/Reflection.h"
#include "gfx/Camera.h"

namespace BHive
{
	DECLARE_ENUM()
	enum class EProjectionType
	{
		Perspective,
		Orthographic
	};

	DECLARE_STRUCT()
	struct FPerspectiveSettings
	{
		DECLARE_PROPERTY()
		float Fov = 45.0f;

		DECLARE_PROPERTY()
		float Near = 0.01f;

		DECLARE_PROPERTY()
		float Far = 1000.0f;

		template <typename A>
		void Serialize(A &ar)
		{
			ar(Fov, Near, Far);
		}
	};

	DECLARE_STRUCT()
	struct FOrthographicSettings
	{
		DECLARE_PROPERTY()
		float Left = -10.0f;

		DECLARE_PROPERTY()
		float Right = 10.f;

		DECLARE_PROPERTY()
		float Bottom = -10.f;

		DECLARE_PROPERTY()
		float Top = 10.0f;

		DECLARE_PROPERTY()
		float Near = -1.0f;

		DECLARE_PROPERTY()
		float Far = 1.0f;

		template <typename A>
		void Serialize(A &ar)
		{
			ar(Left, Right, Bottom, Top, Near, Far);
		}
	};

	DECLARE_CLASS()
	class SceneCamera : public Camera
	{
	public:
	public:
		SceneCamera();

		DECLARE_FUNCTION()
		void SetProjectionType(const EProjectionType &projection);

		DECLARE_FUNCTION()
		const EProjectionType &GetProjectionType() const { return mProjectionType; }

		void SetPerspective(float fov, float aspect, float nearclip, float farclip);
		void SetOrthographic(float size, float aspect, float nearclip, float farclip);
		void SetOrthographic(float l, float r, float b, float t, float aspect, float nearclip, float farclip);

		DECLARE_FUNCTION()
		void SetPerspectiveSettings(const FPerspectiveSettings &settings);

		DECLARE_FUNCTION()
		const FPerspectiveSettings &GetPerspectiveSettings() const { return mPerspectiveSettings; }

		DECLARE_FUNCTION()
		void SetOrthographicSettings(const FOrthographicSettings &settings);

		DECLARE_FUNCTION()
		const FOrthographicSettings &GetOrthographicSettings() const { return mOrthographicSettings; }

		void Resize(uint32_t width, uint32_t height);

		template <typename A>
		void Serialize(A &ar)
		{
			ar(mProjectionType, mPerspectiveSettings, mOrthographicSettings);
		}

	private:
		void RecalculateProjection();

	private:
		DECLARE_PROPERTY(Setter = SetProjectionType, Getter = GetProjectionType)
		EProjectionType mProjectionType = EProjectionType::Perspective;

		DECLARE_PROPERTY(Setter = SetPerspectiveSettings, Getter = GetPerspectiveSettings)
		FPerspectiveSettings mPerspectiveSettings;

		DECLARE_PROPERTY(Setter = SetOrthographicSettings, Getter = GetOrthographicSettings)
		FOrthographicSettings mOrthographicSettings;
		float mAspectRatio = 1.7555f;
	};

} // namespace BHive