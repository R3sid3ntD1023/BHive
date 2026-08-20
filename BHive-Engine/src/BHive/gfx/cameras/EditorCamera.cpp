#include "core/events/Event.h"
#include "EditorCamera.h"
#include "input/InputManager.h"

namespace BHive
{
	EditorCamera::EditorCamera(float fov, float aspect, float _near, float _far)
	{
		SetPerspective(fov, aspect, _near, _far);
	}

	EditorCamera::EditorCamera(float l, float r, float b, float t, float aspect, float _near, float _far)
	{
		SetOrthographic(l, r, b, t, aspect, _near, _far);
	}

	void EditorCamera::ResetOrientation()
	{
		mYaw = -90.f;
		mPitch = 0.f;

		glm::vec3 forward = GetForward();
		mPosition = mTarget - forward * mDistanceToTarget;
	}

	void EditorCamera::ResetView()
	{
		mPosition = {0, 10.f, 10.f};
		mTarget = {0.f, 0.f, 0.f};

		mYaw = -90.f;
		mPitch = 0.f;

		mDistanceToTarget = glm::length(mPosition - mTarget);
	}

	void EditorCamera::FreeFlyMove(const glm::vec3 &direction)
	{
		mPosition += direction * MovementSpeed();
		mTarget += direction * MovementSpeed();
	}

	void EditorCamera::Focus(const glm::vec3 &target, const glm::vec3 &bounds)
	{
		mTarget = target;

		float radius = glm::length(bounds);
		mDistanceToTarget = std::max(radius * 2.0f, 1.0f);

		ResetOrientation();
	}

	void EditorCamera::SetStartState(const glm::vec3 &position, float yaw, float pitch)
	{
		mPosition = position;
		mYaw = yaw;
		mPitch = glm::clamp(pitch, -89.f, 89.f);

		auto forward = GetForward();
		mTarget = position + forward * mDistanceToTarget;
	}

	const glm::mat4 EditorCamera::GetView() const
	{
		return glm::lookAt(mPosition, mTarget, glm::vec3(0, 1, 0));
	}

	void EditorCamera::Resize(uint32_t w, uint32_t h)
	{
		if (w == 0 || h == 0)
			return;

		SceneCamera::Resize(w, h);
		mViewportSize = {w, h};
	}

	glm::vec3 EditorCamera::GetForward() const
	{
		glm::vec3 dir;
		dir.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
		dir.y = sin(glm::radians(mPitch));
		dir.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
		dir = glm::normalize(dir);
		return dir;
	}

	glm::vec3 EditorCamera::GetRight() const
	{
		return glm::normalize(glm::cross(GetForward(), glm::vec3(0, 1, 0)));
	}

	glm::vec3 EditorCamera::GetUp() const
	{
		return glm::normalize(glm::cross(GetRight(), GetForward()));
	}

	void EditorCamera::Zoom(float delta)
	{
		float zoomFactor = powf(0.95f, delta);
		mDistanceToTarget *= zoomFactor;
		mDistanceToTarget = std::max(mDistanceToTarget, 0.1f);

		glm::vec3 forward = GetForward();
		mPosition = mTarget - forward * mDistanceToTarget;
	}

	void EditorCamera::Pan(const glm::vec2 &delta)
	{
		float panSpeed = mDistanceToTarget * 0.002f;

		glm::vec3 right = GetRight();
		glm::vec3 up = GetUp();

		mPosition -= right * delta.x * panSpeed;
		mPosition += up * delta.y * panSpeed;

		mTarget -= right * delta.x * panSpeed;
		mTarget += up * delta.y * panSpeed;
	}

	void EditorCamera::Rotate(const glm::vec2 &delta)
	{
		mYaw += delta.x * RotationSpeed();
		mPitch += delta.y * RotationSpeed();

		mPitch = glm::clamp(mPitch, -89.0f, 89.0f);

		auto dir = GetForward();
		mPosition = mTarget - dir * mDistanceToTarget;
	}

} // namespace BHive