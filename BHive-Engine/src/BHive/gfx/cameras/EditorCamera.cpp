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

	void EditorCamera::ProcessInput()
	{
		if (mAltPressed)
			return;

		auto &input = InputManager::GetInputManager();

		auto forward = mTransform.GetForward();
		auto right = mTransform.GetRight();
		auto delta = input.get_mouse_delta() * .003f;

		if (input.is_pressed(Key::Left_Alt) || input.is_pressed(Key::Right_Alt))
		{
			if (input.is_pressed(Mouse::MouseButtonMiddle))
			{
				Pan(delta);
			}

			else if (input.is_pressed(Mouse::MouseButtonLeft))
			{
				Rotate(delta);
			}

			else if (input.is_pressed(Mouse::MouseButtonRight))
			{
				Zoom(delta.y);
			}

			if (input.is_pressed(Key::Up) || input.is_pressed(Key::W))
			{
				mTransform.AddTranslation(forward * MovementSpeed());
			}

			if (input.is_pressed(Key::Down) || input.is_pressed(Key::S))
			{
				mTransform.AddTranslation(-forward * MovementSpeed());
			}

			if (input.is_pressed(Key::Left) || input.is_pressed(Key::A))
			{
				mTransform.AddTranslation(right * MovementSpeed());
			}

			if (input.is_pressed(Key::Right) || input.is_pressed(Key::D))
			{
				mTransform.AddTranslation(-right * MovementSpeed());
			}
		}
	}

	void EditorCamera::OnEvent(Event &event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch(this, &EditorCamera::OnMouseScrolled);
		dispatcher.Dispatch(this, &EditorCamera::OnKeyEvent);
	}

	bool EditorCamera::OnMouseScrolled(MouseScrolledEvent &event)
	{
		Zoom(event.y * 0.1f);

		return false;
	}

	bool EditorCamera::OnKeyEvent(KeyEvent &e)
	{
		switch (e.Key)
		{
		case Key::Home:
		{
			mTransform = mInitialTransform;
			return true;
		}
		default:
			break;
		}
		return false;
	}

	void EditorCamera::Focus(const FTransform &target, const glm::vec3 &bounds)
	{
		auto target_location = target.GetTranslation();
		auto eye = target_location + (bounds * 1.1f);
		mTransform = glm::inverse(glm::lookAt(eye, target_location, {0, 1, 0}));
	}

	const FTransform &EditorCamera::GetView() const
	{
		return mTransform;
	}

	void EditorCamera::SetView(const FTransform &view)
	{
		mTransform = view;
	}

	void EditorCamera::Resize(uint32_t w, uint32_t h)
	{
		SceneCamera::Resize(w, h);
		mViewportSize = {w, h};
	}

	glm::vec2 EditorCamera::PanSpeed() const
	{
		float x = std::min(mViewportSize.x / 1000.0f, 2.4f);
		float xFentity = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(mViewportSize.y / 1000.0f, 2.4f);
		float yFentity = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return {xFentity, yFentity};
	}

	float EditorCamera::RotationSpeed() const
	{
		return 50.0f;
	}

	float EditorCamera::MovementSpeed() const
	{
		return .1f;
	}

	float EditorCamera::Distance() const
	{
		return glm::length(mTransform.GetTranslation());
	}

	void EditorCamera::Zoom(float delta)
	{
		auto offset = delta * ZoomSpeed();
		auto new_position = mTransform.GetForward() * offset + mTransform.GetTranslation();

		if (Distance() < 1.0f)
		{
			mTarget -= mTransform.GetForward();
		}

		mTransform.SetTranslation(new_position);
	}

	void EditorCamera::Pan(const glm::vec2 &delta)
	{
		float distance = Distance();

		auto speed = PanSpeed();
		auto translation = mTransform.GetTranslation();
		auto deltax = mTransform.GetRight() * delta.x * speed.x * distance;
		auto deltay = mTransform.GetUp() * delta.y * speed.y * distance;

		mTransform.SetTranslation(translation + deltax + deltay);
	}

	void EditorCamera::Rotate(const glm::vec2 &delta)
	{
		float yaw_sign = mTransform.GetUp().y < 0 ? -1.0f : 1.0f;
		auto yaw = yaw_sign * delta.x * RotationSpeed();
		auto pitch = delta.y * RotationSpeed();

		auto rotation = mTransform.GetRotation();
		mTransform.SetRotation(rotation + glm::vec3{pitch, yaw, 0.0f});
	}

	float EditorCamera::ZoomSpeed() const
	{
		float distance = Distance();
		float speed = distance * distance;
		speed = std::min(speed, 100.0f);
		return speed;
	}

} // namespace BHive