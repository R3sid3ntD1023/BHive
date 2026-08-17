#include "InputManager.h"
#include "core/Application.h"
#include <glfw/glfw3.h>

namespace BHive
{
	void InputManager::BeginFrame()
	{
		mScrollDelta = glm::vec2(0, 0);
		mMouseDelta = glm::vec2(0, 0);
	}

	void InputManager::EndFrame()
	{
		mPrevMousePos = mMousePos;
		mPreviousKeys = mCurrentKeys;
		mPreviousMouse = mCurrentMouse;
	}

	void InputManager::OnKeyEvent(uint32_t code, EventStatusCode action, ModCode mods)
	{
		mCurrentKeys[code] = action;
	}

	void InputManager::OnMouseEvent(uint32_t code, EventStatusCode action, ModCode mods)
	{
		mCurrentMouse[code] = action;
	}

	void InputManager::OnScrollEvent(float x, float y)
	{
		mScrollDelta = {x, y};
	}

	void InputManager::OnMouseMove(float x, float y)
	{
		mMousePos = {x, y};
		mMouseDelta = mMousePos - mPrevMousePos;
	}

	bool InputManager::IsPressed(uint32_t code) const
	{
		if (code < Mouse::MouseButtonLast)
			return mCurrentMouse.contains(code) && (mCurrentMouse.at(code) != EventStatus::RELEASE);

		return mCurrentKeys.contains(code) && (mCurrentKeys.at(code) != EventStatus::RELEASE);
	}

	bool InputManager::IsReleased(uint32_t code) const
	{
		return !IsPressed(code);
	}

	bool InputManager::IsPressedOnce(uint32_t code) const
	{
		bool now = IsPressed(code);
		bool before = (code < Mouse::MouseButtonLast) ? (mPreviousMouse.contains(code) && mPreviousMouse.at(code) != EventStatus::RELEASE)
													  : (mPreviousKeys.contains(code) && mPreviousKeys.at(code) != EventStatus::RELEASE);
		return now && !before;
	}

	bool InputManager::IsReleasedOnce(uint32_t code) const
	{
		bool now = IsPressed(code);
		bool before = (code < Mouse::MouseButtonLast) ? (mPreviousMouse.contains(code) && mPreviousMouse.at(code) != EventStatus::RELEASE)
													  : (mPreviousKeys.contains(code) && mPreviousKeys.at(code) != EventStatus::RELEASE);
		return !now && before;
	}

	EventStatusCode InputManager::GetState(uint32_t code) const
	{
		return mPreviousKeys.contains(code) ? mPreviousKeys.at(code) : EventStatus::RELEASE;
	}

	InputManager &InputManager::Get()
	{
		static InputManager manager;
		return manager;
	}
} // namespace BHive