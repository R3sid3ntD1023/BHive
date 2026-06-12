#include "Color.h"

namespace BHive
{
	FColor::FColor()
		: r(0.0f),
		  g(0.0f),
		  b(0.0f),
		  a(1.0f)
	{
	}

	FColor::FColor(const FColor &other)
	{
		r = other.r;
		g = other.g;
		b = other.b;
		a = other.a;
	}

	FColor::FColor(uint32_t hex)
	{
		const float s = 1.0f / 255.0f;
		a = ((hex >> 24) & 0xFF) * s;
		r = ((hex >> 16) & 0xFF) * s;
		g = ((hex >> 8) & 0xFF) * s;
		b = (hex & 0xFF) * s;
	}

	FColor::FColor(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a)
		: r(_r / 255.f),
		  g(_g / 255.f),
		  b(_b / 255.f),
		  a(_a)
	{
	}

	FColor::FColor(float v)
		: r(v),
		  g(v),
		  b(v),
		  a(1.0f)
	{
	}

	FColor::FColor(float _r, float _g, float _b, float _a)
		: r(_r),
		  g(_g),
		  b(_b),
		  a(_a)
	{
	}

	FColor &FColor::operator=(const FColor &rhs)
	{
		r = rhs.r;
		g = rhs.g;
		b = rhs.b;
		a = rhs.a;
		return *this;
	}

	float &FColor::operator[](int index)
	{
		ASSERT(index >= 0 && index < 4);
		return c[index];
	}

	float FColor::operator[](int index) const
	{
		ASSERT(index >= 0 && index < 4);
		return c[index];
	}

	bool FColor::operator==(const FColor &rhs) const
	{
		return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
	}

	FColor::operator glm::vec4() const
	{
		return {r, g, b, a};
	}

	FColor::operator glm::vec3() const
	{
		return {r, g, b};
	}

	FColor::operator uint32_t() const
	{
		return uint32_t(a * 255) << 24 | uint32_t(255 * r) << 16 | uint32_t(255 * g) << 8 | uint32_t(255 * b) << 0;
	}

	FColor::operator float *()
	{
		return &r;
	}

	FColor::operator const float *() const
	{
		return &r;
	}

	std::string FColor::to_string() const
	{
		return std::format("[{}, {}, {}, {}]", r, g, b, a);
	}

	//0xAARRGGBB
	const FColor FColor::Blue = 0xFF0000FF;
	const FColor FColor::Red = 0xFFFF0000;
	const FColor FColor::Green = 0xFF00FF00;
	const FColor FColor::White = 0xFFFFFFFF;
	const FColor FColor::Black = 0xFF000000;
	const FColor FColor::Yellow = 0xFFFFFF00;
	const FColor FColor::Magenta = 0xFFFF00FF;
	const FColor FColor::Cyan = 0xFF00FFFF;
	const FColor FColor::Gray = 0xFF808080;
	const FColor FColor::DarkGray = 0xFF404040;
	const FColor FColor::LightGray = 0xFFC0C0C0;
	const FColor FColor::Orange = 0xFFFFA500;
	const FColor FColor::Pink = 0xFFFFC0CB;
	const FColor FColor::Purple = 0xFF800080;
	const FColor FColor::Brown = 0xFFA52A2A;

} // namespace BHive