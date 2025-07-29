#pragma once

#include "core/Core.h"
#include "gui/GUICore.h"
#include "core/math/Math.h"

namespace BHive
{
	struct BHIVE_API FColor
	{
		float r = 0, g = 0, b = 0, a = 1;

		FColor() = default;

		FColor(const FColor &);

		FColor(uint32_t hex);

		FColor(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a = (uint8_t)255);

		FColor(float v);

		FColor(float _r, float _g, float _b, float _a = 1.0f);

		FColor &operator=(const FColor &rhs);

		bool operator==(const FColor &rhs) const;

		operator ImVec4() const;

		operator glm::vec4() const;

		operator glm::vec3() const;

		operator uint32_t() const;

		operator float *();

		operator const float *() const;

		std::string to_string() const;

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