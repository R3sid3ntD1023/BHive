#include "EditorCameraController.h"

namespace BHive
{
	void EditorCameraController::Update(float dt)
	{
		if (!mCamera)
			return;

		auto &input = InputManager::Get();
		glm::vec2 mouseDelta = input.GetMouseDelta();
		glm::vec2 scrollDelta = input.GetScrollDelta();

		bool alt = input.IsPressed(Key::Left_Alt) || input.IsPressed(Key::Right_Alt);

		// ORBIT / PAN /ZOOM
		if (alt)
		{
			if (input.IsPressed(Mouse::MouseButtonLeft))
				mCamera->Rotate(mouseDelta);

			if (input.IsPressed(Mouse::MouseButtonMiddle))
				mCamera->Pan(mouseDelta);

			if (input.IsPressed(Mouse::MouseButtonRight))
				mCamera->Zoom(mouseDelta.y);

			if (scrollDelta.y != 0.0f)
			{
				mCamera->Zoom(scrollDelta.y);
			}
		}
		else
		{
			// FREE FLY
			glm::vec3 moveDir{0};

			if (input.IsPressed(Key::W))
				moveDir += mCamera->GetForward();
			if (input.IsPressed(Key::S))
				moveDir -= mCamera->GetForward();
			if (input.IsPressed(Key::A))
				moveDir -= mCamera->GetRight();
			if (input.IsPressed(Key::D))
				moveDir += mCamera->GetRight();

			if (moveDir != glm::vec3(0))
				mCamera->FreeFlyMove(moveDir);
		}

		// SHORTCUTS
		if (input.IsPressedOnce(Key::F))
			mCamera->Focus(mSelectedTransform, mSelectedBounds);

		if (input.IsPressedOnce(Key::R))
			mCamera->ResetOrientation();

		if (input.IsPressedOnce(Key::Home))
			mCamera->ResetView();
	}
} // namespace BHive