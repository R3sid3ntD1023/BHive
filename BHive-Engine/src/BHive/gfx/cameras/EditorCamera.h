#pragma once

#include "SceneCamera.h"

namespace BHive
{
	struct CameraSensitivity
	{
		float MoveSpeed = 10.0, ZoomSpeed = 2.0, PanSpeed = 1.0, OrbitSpeed = 1.0;
	};

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

		void SetStartState(const glm::vec3 &position, float yaw, float pitch);

		const glm::mat4 GetView() const;

		void ResetOrientation();

		void ResetView();

		void FreeFlyMove(const glm::vec3 &direction);

		void Resize(uint32_t w, uint32_t h) override;

		void SetSensitivity(CameraSensitivity sensitivity) { mSensitivity = sensitivity; }

		glm::vec3 GetForward() const;

		glm::vec3 GetRight() const;

		glm::vec3 GetUp() const;

	public:
		const CameraSensitivity &GetSensitivy() const { return mSensitivity; };

	private:
		float mYaw = -90.f;

		float mPitch = 0.0f;

		float mDistanceToTarget = 10.f;

		glm::vec3 mPosition{0.0f, 10.0f, 10.0f};

		glm::vec3 mTarget = glm::vec3(0.0f);

		glm::vec2 mViewportSize{0, 0};

		CameraSensitivity mSensitivity{};
	};
} // namespace BHive