#include "Transform.h"

namespace BHive
{
	FTransform::FTransform()
		: FTransform({0, 0, 0}, {0, 0, 0}, {1, 1, 1})
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

	FTransform::FTransform(const FTransform &rhs)
		: mTranslation(rhs.mTranslation),
		  mRotation(rhs.mRotation),
		  mScale(rhs.mScale)
	{
		calculate_model_matrix();
	}

	FTransform::FTransform(
		const glm::vec3 &translation, const glm::vec3 &rotation, const glm::vec3 &scale)
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
			   std::format(
				   "{},{},{} ", glm::to_string(mTranslation), glm::to_string(mRotation),
				   glm::to_string(mScale)) +
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

	void FTransform::calculate_model_matrix()
	{
		auto rotation = glm::quat(glm::radians(mRotation));
		mModelMatrix = glm::translate(mTranslation) * glm::toMat4(rotation) * glm::scale(mScale);
	}

	REFLECT(FTransform)
	{
		BEGIN_REFLECT(FTransform)
		REFLECT_PROPERTY("Translation", get_translation, set_translation)
		REFLECT_PROPERTY("Rotation", get_rotation, set_rotation)
		REFLECT_PROPERTY("Scale", get_scale, set_scale);
	}
} // namespace BHive
