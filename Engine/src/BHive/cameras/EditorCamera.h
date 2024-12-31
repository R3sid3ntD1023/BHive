#pragma once

#include "gfx/Camera.h"
#include "math/Transform.h"
#include "events/MouseEvents.h"
#include "events/InputEvents.h"

namespace BHive
{
	class Event;

	class EditorCamera : public Camera
	{
	public:
		EditorCamera();
		EditorCamera(float fov, float aspect, float _near, float _far);

		void ProcessInput();

		void Resize(uint32_t width, uint32_t height);
		void OnEvent(Event& event);

		bool OnMouseScrolled(MouseScrolledEvent& event);

		void Focus(const FTransform& target, const glm::vec3& distance);
		void SetView(const FTransform& view);

	public:
		const FTransform& GetView() const;
		
		float ZoomSpeed() const;
		glm::vec2 PanSpeed() const;
		float RotationSpeed() const;
		float Distance() const;

	private:
		void Zoom(float delta);
		void Pan(const glm::vec2& delta);
		void Rotate(const glm::vec2& delta);

		void RecalulatePojection();

	private:
		float mFov = 45.f, mAspect = 1.7555f, mNear = 0.1f, mFar = 1000.0f;
		FTransform mTransform{ {0.0f, 10.0f, 10.0f}, {-45, 0, 0} };
		glm::vec3 mTarget{ 0.0f };
		glm::vec2 mPrevMousePosition{ 0,0 };
		glm::vec2 mViewportSize{ 0,0 };

		bool mAltPressed{ false };
		std::map<MouseCode, bool> mButtonStatus;
	};
}