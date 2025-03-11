#pragma once

#include "core/events/KeyEvents.h"
#include "core/events/MouseEvents.h"
#include "math/Transform.h"
#include "PerspectiveCamera.h"

namespace BHive
{
	class Event;

	class EditorCamera : public PerspectiveCamera
	{
	public:
		EditorCamera() = default;
		EditorCamera(float fov, float aspect, float _near, float _far);

		void ProcessInput();
		void OnEvent(Event &event);

		bool OnMouseScrolled(MouseScrolledEvent &event);
		bool OnKeyEvent(KeyEvent &e);

		void Focus(const FTransform &target, const glm::vec3 &distance);

		virtual const glm::mat4 &GetView() const;

		virtual void Resize(uint32_t w, uint32_t h) override;

	public:
		float ZoomSpeed() const;
		glm::vec2 PanSpeed() const;
		float RotationSpeed() const;
		float MovementSpeed() const;
		float Distance() const;

	private:
		void Zoom(float delta);
		void Pan(const glm::vec2 &delta);
		void Rotate(const glm::vec2 &delta);

	private:
		FTransform mInitialTransform{{0.0f, 10.0f, 10.0f}, {-45, 0, 0}};
		FTransform mTransform{mInitialTransform};
		glm::vec3 mTarget{0.0f};
		glm::vec2 mPrevMousePosition{0, 0};
		glm::vec2 mViewportSize{0, 0};

		bool mAltPressed{false};
		std::map<MouseCode, bool> mButtonStatus;
	};
} // namespace BHive