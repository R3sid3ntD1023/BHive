#pragma once

#include "SceneCamera.h"
#include "core/events/KeyEvents.h"
#include "core/events/MouseEvents.h"

namespace BHive
{
	class Event;

	class BHIVE_API EditorCamera : public SceneCamera
	{

	public:
		EditorCamera() = default;

		EditorCamera(float fov, float aspect, float _near, float _far);

		EditorCamera(float l, float r, float b, float t, float aspect, float _near, float _far);

		void Zoom(float delta);

		void Pan(const glm::vec2 &delta);

		void Rotate(const glm::vec2 &delta);

		void Focus(const glm::vec3 &target, const glm::vec3 &bounds = {});

		void SetPosition(const glm::vec3 &position) { mPosition = position; }

		void SetYaw(float yaw) { mYaw = yaw; }

		void SetPitch(float pitch) { mPitch = pitch; }

		const glm::mat4 GetView() const;

		void ResetOrientation();

		void ResetView();

		void FreeFlyMove(const glm::vec3 &direction);

		void Resize(uint32_t w, uint32_t h) override;

		glm::vec3 GetForward() const;

		glm::vec3 GetRight() const;

		glm::vec3 GetUp() const;

	public:
		float RotationSpeed() const { return 0.1f; };

		float MovementSpeed() const { return 1.0f; };

	private:
		float mYaw = -90.f;

		float mPitch = 0.0f;

		float mDistanceToTarget = 10.f;

		glm::vec3 mPosition{0.0f, 10.0f, 10.0f};

		glm::vec3 mTarget = glm::vec3(0.0f);

		glm::vec2 mViewportSize{0, 0};
	};
} // namespace BHive