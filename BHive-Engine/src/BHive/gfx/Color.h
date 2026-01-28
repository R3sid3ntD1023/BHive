#pragma once

#include "core/Core.h"
#include "gui/GUICore.h"
#include "core/math/Math.h"

namespace BHive
{
	struct BHIVE_API FColor
	{
		union
		{
			struct
			{
				float r, g, b, a;
			};

			float c[4];
		};

		FColor();

		FColor(const FColor &);

		FColor(uint32_t hex);

		FColor(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a = (uint8_t)255);

		FColor(float v);

		FColor(float _r, float _g, float _b, float _a = 1.0f);

		float &operator[](int index);

		float operator[](int index) const;

		FColor &operator=(const FColor &rhs);

		bool operator==(const FColor &rhs) const;

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

		// predefined colors
		static const FColor Blue;
		static const FColor Red;
		static const FColor Green;
		static const FColor White;
		static const FColor Black;
		static const FColor Yellow;
		static const FColor Magenta;
		static const FColor Cyan;
		static const FColor Gray;
		static const FColor DarkGray;
		static const FColor LightGray;
		static const FColor Orange;
		static const FColor Pink;
		static const FColor Purple;
		static const FColor Brown;
	};
} // namespace BHive