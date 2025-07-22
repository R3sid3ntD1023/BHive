#include "Transform.h"

namespace BHive
{
	FTransform::FTransform()
		: FTransform({0, 0, 0}, {0, 0, 0}, {1, 1, 1})
	{
	}

	FTransform::FTransform(const glm::vec3 &translation, const glm::quat &rotation)
		: FTransform(translation, glm::degrees(glm::eulerAngles(rotation)))
	{
	}

	FTransform::FTransform(const glm::mat4 &matrix)
	{
		mModelMatrix = matrix;

		glm::quat rotation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(mModelMatrix, mScale, rotation, mTranslation, skew, perspective);

		mRotation = glm::degrees(glm::eulerAngles(rotation));
	}

	FTransform::FTransform(const glm::vec3 &translation, const glm::vec3 &rotation, const glm::vec3 &scale)
		: mTranslation(translation),
		  mRotation(rotation),
		  mScale(scale),
		  mModelMatrix(1.0f)
	{
		calculate_model_matrix();
	}

	void FTransform::set_translation(const glm::vec3 &translation)
	{
		mTranslation = translation;
		calculate_model_matrix();
	}

	void FTransform::set_translation(float x, float y, float z)
	{
		set_translation({x, y, z});
	}

	void FTransform::set_rotation(const glm::vec3 &rotation)
	{
		mRotation = rotation;
		calculate_model_matrix();
	}

	void FTransform::set_quaternion(const glm::quat &quaternion)
	{
		mRotation = glm::degrees(glm::eulerAngles(quaternion));
		calculate_model_matrix();
	}

	void FTransform::set_scale(const glm::vec3 &scale)
	{
		mScale = scale;
		calculate_model_matrix();
	}

	void FTransform::add_translation(const glm::vec3 &translation)
	{
		mTranslation += translation;
		calculate_model_matrix();
	}

	void FTransform::add_rotation(const glm::vec3 &rotation)
	{
		mRotation += rotation;
		calculate_model_matrix();
	}

	glm::vec3 FTransform::get_forward() const
	{
		return glm::normalize(mModelMatrix[2]);
	}

	glm::vec3 FTransform::get_forward_unnormalized() const
	{
		return -mModelMatrix[2];
	}

	glm::vec3 FTransform::get_right() const
	{
		return -glm::normalize(mModelMatrix[0]);
	}

	glm::vec3 FTransform::get_up() const
	{
		return glm::normalize(mModelMatrix[1]);
	}

	FTransform FTransform::inverse() const
	{
		return FTransform(glm::inverse(mModelMatrix));
	}

	std::string FTransform::to_string() const
	{
		return "{" +
			   std::format("{},{},{} ", glm::to_string(mTranslation), glm::to_string(mRotation), glm::to_string(mScale)) +
			   "}";
	}

	FTransform &FTransform::operator=(const FTransform &rhs)
	{
		mTranslation = rhs.mTranslation;
		mRotation = rhs.mRotation;
		mScale = rhs.mScale;

		calculate_model_matrix();

		return *this;
	}

	FTransform FTransform::operator*(const FTransform &rhs) const
	{
		auto new_matrix = mModelMatrix * rhs.mModelMatrix;

		return FTransform(new_matrix);
	}

	FTransform FTransform::operator+(const FTransform &rhs) const
	{
		auto t = mTranslation + rhs.mTranslation;
		auto r = mRotation + rhs.mRotation;
		auto s = mScale * rhs.mScale;
		return {t, r, s};
	}

	FTransform &FTransform::operator+=(const FTransform &rhs)
	{
		return *this = *this + rhs;
	}

	FTransform FTransform::operator/(float rhs) const
	{
		auto t = mTranslation / rhs;
		auto r = mRotation / rhs;
		auto s = mScale / rhs;
		return {t, r, s};
	}

	FTransform &FTransform::operator/=(float rhs)
	{
		return *this = *this / rhs;
	}

	void FTransform::calculate_model_matrix()
	{
		auto rotation = glm::quat(glm::radians(mRotation));
		mModelMatrix = glm::translate(mTranslation) * glm::toMat4(rotation) * glm::scale(mScale);
	}

} // namespace BHive
