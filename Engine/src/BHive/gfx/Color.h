#pragma once

#include "math/Math.h"
#include <imgui.h>
#include "serialization/Serialization.h"

namespace BHive
{
	struct BHIVE Color
	{
		Color() = default;
		Color(const Color &) = default;

		Color(uint32_t hex)
		{
			const float s = 1.0f / 255.0f;
			r = ((hex >> 0) & 0xFF) * s;
			g = ((hex >> 8) & 0xFF) * s;
			b = ((hex >> 16) & 0xFF) * s;
			a = ((hex >> 24) & 0xFF) * s;
		}

		Color(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a = (uint8_t)255)
			: r(_r / 255.f), g(_g / 255.f), b(_b / 255.f), a(_a)
		{
		}

		Color(float v)
			: r(v), g(v), b(v), a(1.0f)
		{
		}

		Color(float _r, float _g, float _b, float _a = 1.0f)
			: r(_r), g(_g), b(_b), a(_a)
		{
		}

		Color &operator=(const Color &rhs)
		{
			r = rhs.r;
			g = rhs.g;
			b = rhs.b;
			a = rhs.a;
			return *this;
		}

		Color &operator=(uint32_t rhs)
		{
			const float s = 1.0f / 255.0f;
			r = ((rhs >> 0) & 0xFF) * s;
			g = ((rhs >> 8) & 0xFF) * s;
			b = ((rhs >> 16) & 0xFF) * s;
			a = ((rhs >> 24) & 0xFF) * s;
			return *this;
		}

		float r = 0, g = 0, b = 0, a = 1;

		operator ImVec4() const { return ImVec4(r, g, b, a); }

		operator glm::vec4() const { return {r, g, b, a}; }

		operator glm::vec3() const { return {r, g, b}; }

		operator uint32_t() const { return uint32_t(a * 255) << 24 | uint32_t(255 * g) << 16 | uint32_t(255 * b) << 8 | uint32_t(255 * a) << 0; }

		operator float *() { return &r; }

		operator const float *() const { return &r; }

		std::string to_string()
		{
			std::stringstream ss;
			ss << "{" << r << "," << g << "," << b << "," << a << "}";
			return ss.str();
		}

		void Serialize(StreamWriter &ar) const;
		void Deserialize(StreamReader &ar);
	};

	namespace Colors
	{
		static const Color Blue = 0xFFFF0000;
		static const Color Red = 0xFF0000FF;
		static const Color Green = 0xFF00FF00;
		static const Color White = 0xFFFFFFFF;
	}
}