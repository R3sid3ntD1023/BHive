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
		using INPUT_MAP = std::unordered_map<uint32_t, EventStatusCode>;

		void add_input(uint32_t code, EventStatusCode action, ModCode mods);
		void set_scroll(float x, float y);

		EventStatusCode get_input_state(uint32_t code) const;
		bool is_pressed(uint32_t code) const;

		const glm::vec2 get_scroll() const { return mScroll; }
		const glm::vec2 get_mouse_pos() const;
		const glm::vec2 get_mouse_delta() const;

		static InputManager &GetInputManager();

	private:
		INPUT_MAP mInputs;
		glm::vec2 mScroll{};
	};
} // namespace BHive