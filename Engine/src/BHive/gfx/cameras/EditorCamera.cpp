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

		auto forward = mTransform.get_forward();
		auto right = mTransform.get_right();
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
		}

		if (input.is_pressed(Key::Up) || input.is_pressed(Key::W))
		{
			mTransform.add_translation(-forward * MovementSpeed());
		}

		if (input.is_pressed(Key::Down) || input.is_pressed(Key::S))
		{
			mTransform.add_translation(forward * MovementSpeed());
		}

		if (input.is_pressed(Key::Left) || input.is_pressed(Key::A))
		{
			mTransform.add_translation(right * MovementSpeed());
		}

		if (input.is_pressed(Key::Right) || input.is_pressed(Key::D))
		{
			mTransform.add_translation(-right * MovementSpeed());
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
		case Key::F:
		{
			mTransform = mInitialTransform;
			return true;
		}
		default:
			break;
		}
		return false;
	}

	void EditorCamera::Focus(const FTransform &target, const glm::vec3 &distance)
	{
		mTarget = target.get_translation();
		auto eye = mTarget + distance * 2.0f;
		auto target_pos = mTarget;
		mTransform = glm::lookAt(eye, target_pos, {0, 1, 0});
	}

	const glm::mat4 &EditorCamera::GetView() const
	{
		return mTransform.inverse();
	}

	void EditorCamera::SetView(const glm::mat4 &view)
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
		return glm::length(mTransform.get_translation());
	}

	void EditorCamera::Zoom(float delta)
	{
		auto offset = delta * ZoomSpeed();
		auto new_position = mTransform.get_translation() + -mTransform.get_forward() * offset;

		if (Distance() < 1.0f)
		{
			mTarget -= mTransform.get_forward();
		}

		mTransform.set_translation(new_position);
	}

	void EditorCamera::Pan(const glm::vec2 &delta)
	{
		float distance = Distance();

		auto speed = PanSpeed();
		auto translation = mTransform.get_translation();
		auto deltax = mTransform.get_right() * delta.x * speed.x * distance;
		auto deltay = mTransform.get_up() * delta.y * speed.y * distance;

		mTransform.set_translation(translation + deltax + deltay);
	}

	void EditorCamera::Rotate(const glm::vec2 &delta)
	{
		float yaw_sign = mTransform.get_up().y < 0 ? -1.0f : 1.0f;
		auto yaw = yaw_sign * delta.x * RotationSpeed();
		auto pitch = delta.y * RotationSpeed();

		auto rotation = mTransform.get_rotation();
		mTransform.set_rotation(rotation + glm::vec3{pitch, yaw, 0.0f});
	}

	float EditorCamera::ZoomSpeed() const
	{
		float distance = Distance();
		float speed = distance * distance;
		speed = std::min(speed, 100.0f);
		return speed;
	}

} // namespace BHive