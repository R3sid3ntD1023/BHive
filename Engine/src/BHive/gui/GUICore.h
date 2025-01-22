#pragma once

#include <glm/vec2.hpp>

#define IM_VEC2_CLASS_EXTRA                           \
	constexpr ImVec2(float v)                         \
		: x(v),                                       \
		  y(v){};                                     \
	constexpr ImVec2(const glm::vec2 &rhs)            \
		: x(rhs.x),                                   \
		  y(rhs.y){};                                 \
	constexpr ImVec2 &operator=(const glm::vec2 &rhs) \
	{                                                 \
		x = rhs.x;                                    \
		y = rhs.y;                                    \
		return *this;                                 \
	};                                                \
	operator glm::vec2() const                        \
	{                                                 \
		return glm::vec2(x, y);                       \
	}

#define IM_VEC4_CLASS_EXTRA                        \
	ImVec4 operator*(float s) const                \
	{                                              \
		return ImVec4(x * s, y * s, z * s, w * s); \
	}                                              \
	ImVec4 &operator*=(float s)                    \
	{                                              \
		x *= s;                                    \
		y *= s;                                    \
		z *= s;                                    \
		w *= s;                                    \
		return *this;                              \
	}                                              \
	ImVec4 operator/(float s) const                \
	{                                              \
		return ImVec4(x / s, y / s, z / s, w / s); \
	}                                              \
	ImVec4 &operator/=(float s)                    \
	{                                              \
		x /= s;                                    \
		y /= s;                                    \
		z /= s;                                    \
		w /= s;                                    \
		return *this;                              \
	}

#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>
