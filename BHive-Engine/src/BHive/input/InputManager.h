#pragma once

#include "core/Core.h"
#include "core/events/KeyCodes.h"
#include "core/events/MouseCodes.h"
#include "InputAction.h"
#include "core/events/EventStatus.h"
#include "core/events/ModCode.h"
#include "core/math/Math.h"

namespace BHive
{
	struct BHIVE_API InputManager
	{
		void BeginFrame();

		void EndFrame();

		void OnKeyEvent(uint32_t code, EventStatusCode action, ModCode mods);

		void OnMouseEvent(uint32_t code, EventStatusCode action, ModCode mods);

		void OnScrollEvent(float x, float y);

		void OnMouseMove(float x, float y);

		bool IsPressed(uint32_t code) const;

		bool IsReleased(uint32_t code) const;

		bool IsPressedOnce(uint32_t code) const;

		bool IsReleasedOnce(uint32_t code) const;

		EventStatusCode GetState(uint32_t code) const;

		const glm::vec2 &GetScrollDelta() const { return mScrollDelta; }

		const glm::vec2 &GetMouseDelta() const { return mMouseDelta; }

		const glm::vec2 &GetMousePosition() const { return mMousePos; }

		static InputManager &Get();

	private:
		std::unordered_map<uint32_t, EventStatusCode> mCurrentKeys;
		std::unordered_map<uint32_t, EventStatusCode> mPreviousKeys;

		std::unordered_map<uint32_t, EventStatusCode> mCurrentMouse;
		std::unordered_map<uint32_t, EventStatusCode> mPreviousMouse;

		glm::vec2 mMousePos{0, 0};
		glm::vec2 mPrevMousePos{0, 0};
		glm::vec2 mMouseDelta{0, 0};

		glm::vec2 mScrollDelta{0, 0};
	};
} // namespace BHive