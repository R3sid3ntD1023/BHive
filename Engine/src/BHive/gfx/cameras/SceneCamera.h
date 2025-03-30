#pragma once

#include "core/reflection/Reflection.h"
#include "gfx/Camera.h"

namespace BHive
{
	REFLECT_ENUM()
	enum class EProjectionType
	{
		Perspective,
		Orthographic
	};

	REFLECT_STRUCT()
	struct FPerspectiveSettings
	{
		REFLECT_PROPERTY()
		float Fov = 45.0f;

		REFLECT_PROPERTY()
		float Near = 0.01f;

		REFLECT_PROPERTY()
		float Far = 1000.0f;

		template <typename A>
		void Serialize(A &ar)
		{
			ar(Fov, Near, Far);
		}
	};

	REFLECT_STRUCT()
	struct FOrthographicSettings
	{
		REFLECT_PROPERTY()
		float Left = -10.0f;

		REFLECT_PROPERTY()
		float Right = 10.f;

		REFLECT_PROPERTY()
		float Bottom = -10.f;

		REFLECT_PROPERTY()
		float Top = 10.0f;

		REFLECT_PROPERTY()
		float Near = -1.0f;

		REFLECT_PROPERTY()
		float Far = 1.0f;

		template <typename A>
		void Serialize(A &ar)
		{
			ar(Left, Right, Bottom, Top, Near, Far);
		}
	};

	REFLECT_CLASS()
	class SceneCamera : public Camera
	{
	public:
	public:
		SceneCamera();

		REFLECT_FUNCTION()
		void SetProjectionType(const EProjectionType &projection);

		REFLECT_FUNCTION()
		const EProjectionType &GetProjectionType() const { return mProjectionType; }

		void SetPerspective(float fov, float aspect, float nearclip, float farclip);
		void SetOrthographic(float size, float aspect, float nearclip, float farclip);
		void SetOrthographic(float l, float r, float b, float t, float aspect, float nearclip, float farclip);

		REFLECT_FUNCTION()
		void SetPerspectiveSettings(const FPerspectiveSettings &settings);

		REFLECT_FUNCTION()
		const FPerspectiveSettings &GetPerspectiveSettings() const { return mPerspectiveSettings; }

		REFLECT_FUNCTION()
		void SetOrthographicSettings(const FOrthographicSettings &settings);

		REFLECT_FUNCTION()
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
		REFLECT_PROPERTY(Setter = SetProjectionType, Getter = GetProjectionType)
		EProjectionType mProjectionType = EProjectionType::Perspective;

		REFLECT_PROPERTY(Setter = SetPerspectiveSettings, Getter = GetPerspectiveSettings)
		FPerspectiveSettings mPerspectiveSettings;

		REFLECT_PROPERTY(Setter = SetOrthographicSettings, Getter = GetOrthographicSettings)
		FOrthographicSettings mOrthographicSettings;
		float mAspectRatio = 1.7555f;
	};

} // namespace BHive