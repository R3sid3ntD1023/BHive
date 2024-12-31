#pragma once

#include <glm/vec2.hpp>

#define IM_VEC2_CLASS_EXTRA                                        \
	constexpr ImVec2(float v) : x(v), y(v){};                      \
	constexpr ImVec2(const glm::vec2 &rhs) : x(rhs.x), y(rhs.y){}; \
	constexpr ImVec2 &operator=(const glm::vec2 &rhs)              \
	{                                                              \
		x = rhs.x;                                                 \
		y = rhs.y;                                                 \
		return *this;                                              \
	};                                                             \
	operator glm::vec2() const { return glm::vec2(x, y); }

#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>
