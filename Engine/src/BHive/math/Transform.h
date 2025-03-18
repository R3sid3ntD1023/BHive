#pragma once

#include "core/reflection/Reflection.h"
#include "math/Math.h"

namespace BHive
{
	struct BHIVE FTransform
	{
		FTransform();
		FTransform(const glm::mat4 &matrix);
		FTransform(const FTransform &rhs);
		FTransform(const glm::vec3 &translation, const glm::vec3 &rotation = {0, 0, 0}, const glm::vec3 &scale = {1, 1, 1});

		const glm::vec3 &get_translation() const { return mTranslation; }
		const glm::vec3 &get_rotation() const { return mRotation; }
		const glm::vec3 &get_scale() const { return mScale; }

		const glm::quat get_quaternion() const { return glm::quat(glm::radians(mRotation)); }

		void set_translation(const glm::vec3 &translation);
		void set_translation(float x, float y, float z);

		void set_rotation(const glm::vec3 &rotation);
		void set_quaternion(const glm::quat &quaternion);

		void set_scale(const glm::vec3 &scale);

		void add_translation(const glm::vec3 &translation);
		void add_rotation(const glm::vec3 &rotation);

		glm::vec3 get_forward() const;
		glm::vec3 get_forward_unnormalized() const;

		glm::vec3 get_right() const;
		glm::vec3 get_up() const;

		FTransform inverse() const;

		const glm::mat4 &to_mat4() const { return mModelMatrix; }
		operator const glm::mat4 &() const { return to_mat4(); }

		std::string to_string() const;

		FTransform &operator=(const FTransform &rhs);
		FTransform operator*(const FTransform &rhs) const;

		FTransform operator+(const FTransform &rhs) const;
		FTransform &operator+=(const FTransform &rhs);

		FTransform operator/(float rhs) const;
		FTransform &operator/=(float rhs);

		template <typename A>
		inline void Save(A &ar) const
		{
			ar(mTranslation, mRotation, mScale);
		}

		template <typename A>
		inline void Load(A &ar)
		{
			ar(mTranslation, mRotation, mScale);
			calculate_model_matrix();
		}

	private:
		void calculate_model_matrix();

	private:
		glm::vec3 mTranslation;
		glm::vec3 mRotation;
		glm::vec3 mScale;
		glm::mat4 mModelMatrix;
	};

	template <typename Ostream>
	inline Ostream &operator<<(Ostream &os, const FTransform &obj)
	{
		os << '{' << obj.get_translation() << ',' << obj.get_rotation() << ',' << obj.get_scale() << '}';
		return os;
	}

	template <typename Istream>
	inline Istream &operator>>(Istream &is, FTransform &obj)
	{
		char token;
		glm::vec3 t, r, s = {};
		is >> token >> t >> token >> r >> token >> s >> token;
		obj = {t, r, s};
		return is;
	}

	template <>
	inline FTransform Math::Lerp(const FTransform &v0, const FTransform &v1, float t)
	{
		auto translation = Math::Lerp(v0.get_translation(), v1.get_translation(), t);
		auto rotation = Math::Lerp(v0.get_rotation(), v1.get_rotation(), t);
		auto scale = Math::Lerp(v0.get_scale(), v1.get_scale(), t);

		return FTransform{translation, rotation, scale};
	}

	inline FTransform mix(const FTransform &t0, const FTransform &t1, float w)
	{
		auto translation = glm::mix(t0.get_translation(), t1.get_translation(), w);
		auto rotation = glm::slerp(t0.get_quaternion(), t1.get_quaternion(), w);
		auto scale = glm::mix(t0.get_scale(), t1.get_scale(), w);

		FTransform t;
		t.set_translation(translation);
		t.set_quaternion(rotation);
		t.set_scale(scale);

		return t;
	}

} // namespace BHive
