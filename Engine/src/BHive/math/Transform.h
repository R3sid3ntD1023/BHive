#pragma once

#include "math/Math.h"
#include "reflection/Reflection.h"

namespace BHive
{
#define DEFAULT_SCALE {1.0f, 1.0f, 1.0f}
#define DEFAULT_ROTATION {0.0f, 0.0f, 0.0f}

	struct BHIVE FTransform
	{
		FTransform();
		FTransform(const glm::mat4 &matrix);
		FTransform(const FTransform &rhs);
		FTransform(const glm::vec3 &translation, const glm::vec3 &rotation = DEFAULT_ROTATION, const glm::vec3 &scale = DEFAULT_SCALE);

		const glm::vec3 &get_translation() const { return Translation; }
		const glm::vec3 &get_rotation() const { return Rotation; }
		const glm::vec3 &get_scale() const { return Scale; }

		const glm::quat get_quaternion() const { return glm::quat(glm::radians(Rotation)); }

		void set_translation(const glm::vec3 &translation);
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

		const glm::mat4 &to_mat4() const { return ModelMatrix; }
		operator const glm::mat4 &() const { return to_mat4(); }

		std::string to_string() const;

		FTransform &operator=(const FTransform &rhs);
		FTransform operator*(const FTransform &rhs) const;

		void Serialize(StreamWriter &ar) const;

		void Deserialize(StreamReader &ar);

		REFLECTABLE()

	private:
		void calculate_model_matrix();

	private:
		glm::vec3 Translation;
		glm::vec3 Rotation;
		glm::vec3 Scale;
		glm::mat4 ModelMatrix;
	};

	REFLECT_EXTERN(FTransform)

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
}
