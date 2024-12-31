#pragma once

#include <stdint.h>

namespace BHive
{
	template<typename T, unsigned L>
	struct Vector;

#define VEC_OPERATORS()\
		Vector& operator+=(const Vector& rhs) { return *this = *this + rhs; }\
		Vector& operator-=(const Vector& rhs) { return *this = *this - rhs; }\
		Vector& operator*=(const Vector& rhs) { return *this = *this * rhs; }\
		Vector& operator/=(const Vector& rhs) { return *this = *this / rhs; }\
		float operator[](unsigned i) const { return data[i]; }\
		const float* operator[](unsigned i) const { return &data[i]; }\
		float* operator[](unsigned i) { return &data[i]; }

	template<typename T>
	struct Vector<T, 2>
	{
		union
		{
			T x, y;
			T data[2] = { 0, 0 };
		};

		Vector() = default;
		Vector(T x, T y) :x(x), y(y) {}

		Vector& operator=(const Vector& rhs) { x = rhs.x; y = rhs.y; return *this; }

		Vector operator+(const Vector& rhs) const { return { x + rhs.x, y + rhs.y }; }
		Vector operator-(const Vector& rhs) const { return { x - rhs.x, y - rhs.y }; }

		Vector operator*(const Vector& rhs) const { return { x * rhs.x, y * rhs.y }; }
		Vector operator/(const Vector& rhs) const { return { x / rhs.x, y / rhs.y }; }

		float length() const { return sqrt(x * x + y * y); }



		VEC_OPERATORS();
	};

	template<typename T>
	struct Vector<T, 3>
	{
		union
		{
			T x, y, z;
			T data[3] = { 0 , 0, 0};
		};

		Vector& operator=(const Vector& rhs) { x = rhs.x; y = rhs.y; z = rhs.z; return *this; }

		Vector operator+(const Vector& rhs) const { return { x + rhs.x, y + rhs.y, z + rhs.z}; }
		Vector operator-(const Vector& rhs) const { return { x - rhs.x, y - rhs.y, z - rhs.z}; }

		Vector operator*(const Vector& rhs) const { return { x * rhs.x, y * rhs.y, z * rhs.z }; }
		Vector operator/(const Vector& rhs) const { return { x / rhs.x, y / rhs.y, z / rhs.z }; }

		float length() const { return sqrt(x * x + y * y + z * z); }

		
		VEC_OPERATORS();
	};

	template<typename T>
	struct Vector<T, 4>
	{
		union
		{
			T x , y , z , w;
			T data[4] = { 0 , 0, 0, 1 };
		};

		Vector& operator=(const Vector& rhs) { x = rhs.x; y = rhs.y; z = rhs.z; w = rhs.z; return *this; }

		Vector operator+(const Vector& rhs) const { return { x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w }; }
		Vector operator-(const Vector& rhs) const { return { x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w }; }

		Vector operator*(const Vector& rhs) const { return { x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w }; }
		Vector operator/(const Vector& rhs) const { return { x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w }; }

		float length() const { return sqrt(x * x + y * y + z * z + w * w); }

		VEC_OPERATORS();
	};

	template<typename T, unsigned L>
	inline float dot(const Vector<T, L>& lhs, const Vector<T, L>& rhs);

	template<typename T>
	inline float dot(const Vector<T,  2>& lhs, const Vector<T, 2>& rhs)
	{
		return (lhs.x * rhs.x) + (lhs.y * rhs.y);
	}

	template<typename T>
	inline float dot(const Vector<T, 3>& lhs, const Vector<T, 3>& rhs)
	{
		return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
	}

	template<typename T>
	inline float dot(const Vector<T, 4>& lhs, const Vector<T, 4>& rhs)
	{
		return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z) + (lhs.w * rhs.w);
	}

	using Vector2 = Vector<float, 2>;
	using Vector3 = Vector<float, 3>;
	using Vector4 = Vector<float, 4>;

	using IVector2 = Vector<int32_t, 2>;
	using IVector4 = Vector<int32_t, 4>;
}