#include "Transform.h"

namespace BHive
{
	FTransform::FTransform()
		: FTransform({0, 0, 0}, {0, 0, 0}, {1, 1, 1})
	{
	}

	FTransform::FTransform(const glm::mat4 &matrix)
		: Translation(0), Rotation(0), Scale(0)
	{
		ModelMatrix = matrix;

		glm::quat rotation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(ModelMatrix, Scale, rotation, Translation, skew, perspective);

		Rotation = glm::degrees(glm::eulerAngles(rotation));
	}

	FTransform::FTransform(const FTransform &rhs)
		: Translation(rhs.Translation), Rotation(rhs.Rotation), Scale(rhs.Scale)
	{
		calculate_model_matrix();
	}

	FTransform::FTransform(const glm::vec3 &translation, const glm::vec3 &rotation, const glm::vec3 &scale)
		: Translation(translation), Rotation(rotation), Scale(scale), ModelMatrix(1.0f)
	{
		calculate_model_matrix();
	}

	void FTransform::set_translation(const glm::vec3 &translation)
	{
		Translation = translation;
		calculate_model_matrix();
	}

	void FTransform::set_rotation(const glm::vec3 &rotation)
	{
		Rotation = rotation;
		calculate_model_matrix();
	}

	void FTransform::set_quaternion(const glm::quat &quaternion)
	{
		Rotation = glm::degrees(glm::eulerAngles(quaternion));
		calculate_model_matrix();
	}

	void FTransform::set_scale(const glm::vec3 &scale)
	{
		Scale = scale;
		calculate_model_matrix();
	}

	void FTransform::add_translation(const glm::vec3 &translation)
	{
		Translation += translation;
		calculate_model_matrix();
	}

	void FTransform::add_rotation(const glm::vec3 &rotation)
	{
		Rotation += rotation;
		calculate_model_matrix();
	}

	glm::vec3 FTransform::get_forward() const
	{
		return glm::normalize(ModelMatrix[2]);
	}

	glm::vec3 FTransform::get_forward_unnormalized() const
	{
		return -ModelMatrix[2];
	}

	glm::vec3 FTransform::get_right() const
	{
		return -glm::normalize(ModelMatrix[0]);
	}

	glm::vec3 FTransform::get_up() const
	{
		return glm::normalize(ModelMatrix[1]);
	}

	FTransform FTransform::inverse() const
	{
		return FTransform(glm::inverse(ModelMatrix));
	}

	std::string FTransform::to_string() const
	{
		std::stringstream ss;
		ss << "{\n " << glm::to_string(Translation) << " , \n"
		   << glm::to_string(Rotation) << " , \n"
		   << glm::to_string(Scale) << " , \n"
		   << " }";

		return ss.str();
	}

	FTransform &FTransform::operator=(const FTransform &rhs)
	{
		Translation = rhs.Translation;
		Rotation = rhs.Rotation;
		Scale = rhs.Scale;

		calculate_model_matrix();

		return *this;
	}

	FTransform FTransform::operator*(const FTransform &rhs) const
	{
		auto new_matrix = ModelMatrix * rhs.ModelMatrix;

		return FTransform(new_matrix);
	}

	void FTransform::Serialize(StreamWriter &ar) const
	{
		ar(Translation, Rotation, Scale);
	}

	void FTransform::Deserialize(StreamReader &ar)
	{
		ar(Translation, Rotation, Scale);
		calculate_model_matrix();
	}

	void FTransform::calculate_model_matrix()
	{
		auto rotation = glm::quat(glm::radians(Rotation));
		ModelMatrix = glm::translate(Translation) * glm::toMat4(rotation) * glm::scale(Scale);
	}

	REFLECT(FTransform)
	{
		BEGIN_REFLECT(FTransform)
		REFLECT_PROPERTY("Translation", get_translation, set_translation)
		REFLECT_PROPERTY("Rotation", get_rotation, set_rotation)
		REFLECT_PROPERTY("Scale", get_scale, set_scale);
	}
}
