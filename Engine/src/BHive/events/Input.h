#pragma once

#include "core/Core.h"
#include "KeyCodes.h"
#include "MouseCodes.h"
#include "InputAction.h"
#include "math/Math.h"

namespace BHive
{
	struct BHIVE Input
	{
		using INPUT_MAP = std::unordered_map<uint32_t, InputActionCode>;

		void add_input(uint32_t code, InputActionCode action, ModCode mods);
		void add_mouse(MouseCode button, InputActionCode action, ModCode mods);
		void set_scroll(float x, float y);

		InputActionCode get_input_state(uint32_t code) const;
		bool is_pressed(uint32_t code) const;

		const glm::vec2 get_scroll() const { return mScroll; }
		const glm::vec2 get_mouse_pos() const;
		const glm::vec2 get_mouse_delta() const;

	private:
		INPUT_MAP mInputs;
		glm::vec2 mScroll;
	};
}