#pragma once

#include "core/Core.h"
#include "gui/GUICore.h"
#include "core/math/Math.h"

namespace BHive
{
	struct BHIVE_API FColor
	{
		FColor() = default;
		FColor(const FColor &) = default;

		FColor(uint32_t hex)
		{
			const float s = 1.0f / 255.0f;
			a = ((hex >> 24) & 0xFF) * s;
			r = ((hex >> 16) & 0xFF) * s;
			g = ((hex >> 8) & 0xFF) * s;
			b = (hex & 0xFF) * s;
		}

		FColor(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a = (uint8_t)255)
			: r(_r / 255.f),
			  g(_g / 255.f),
			  b(_b / 255.f),
			  a(_a)
		{
		}

		FColor(float v)
			: r(v),
			  g(v),
			  b(v),
			  a(1.0f)
		{
		}

		FColor(float _r, float _g, float _b, float _a = 1.0f)
			: r(_r),
			  g(_g),
			  b(_b),
			  a(_a)
		{
		}

		FColor &operator=(const FColor &rhs)
		{
			r = rhs.r;
			g = rhs.g;
			b = rhs.b;
			a = rhs.a;
			return *this;
		}

		FColor &operator=(uint32_t hex)
		{
			const float s = 1.0f / 255.0f;
			a = ((hex >> 24) & 0xFF) * s;
			r = ((hex >> 16) & 0xFF) * s;
			g = ((hex >> 8) & 0xFF) * s;
			b = (hex & 0xFF) * s;
			return *this;
		}

		float r = 0, g = 0, b = 0, a = 1;

		operator ImVec4() const { return ImVec4(r, g, b, a); }

		operator glm::vec4() const { return {r, g, b, a}; }

		operator glm::vec3() const { return {r, g, b}; }

		operator uint32_t() const
		{
			return uint32_t(a * 255) << 24 | uint32_t(255 * r) << 16 | uint32_t(255 * g) << 8 | uint32_t(255 * b) << 0;
		}

		operator float *() { return &r; }

		operator const float *() const { return &r; }

		std::string to_string() const { return std::format("[{}, {}, {}, {}]", r, g, b, a); }

		template <typename A, std::enable_if_t<is_binary_archive_v<A>, bool> = true>
		void Serialize(A &ar)
		{
			ar(r, g, b, a);
		}

		template <typename A, std::enable_if_t<is_json_archive_v<A>, bool> = true>
		void Serialize(A &ar)
		{
			size_t size = 4;
			ar(cereal::make_size_tag(size));
			ar(r, g, b, a);
		}
	};

	namespace Colors
	{
		static const FColor Blue = 0xFF0000FF;
		static const FColor Red = 0xFFFF0000;
		static const FColor Green = 0xFF00FF00;
		static const FColor White = 0xFFFFFFFF;
		static const FColor Black = 0xFF000000;
		static const FColor Yellow = 0xFFFFFF00;
		static const FColor Magenta = 0xFFFF00FF;
		static const FColor Cyan = 0xFF00FFFF;
		static const FColor Gray = 0xFF808080;
		static const FColor DarkGray = 0xFF404040;
		static const FColor LightGray = 0xFFC0C0C0;
		static const FColor Orange = 0xFFFFA500;
		static const FColor Pink = 0xFFFFC0CB;
		static const FColor Purple = 0xFF800080;
		static const FColor Brown = 0xFFA52A2A;

	} // namespace Colors
} // namespace BHive