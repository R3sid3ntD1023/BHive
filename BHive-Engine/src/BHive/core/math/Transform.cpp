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
		glm::decompose(mModelMatrix, mData[2], rotation, mData[0], skew, perspective);

		mData[1] = glm::degrees(glm::eulerAngles(rotation));
	}

	FTransform::FTransform(const glm::vec3 &translation, const glm::vec3 &rotation, const glm::vec3 &scale)
		: mData(),
		  mModelMatrix(1.0f)
	{
		mData[0] = translation;
		mData[1] = rotation;
		mData[2] = scale;
		CalculateModelMatrix();
	}

	const glm::quat FTransform::GetQuaternion() const
	{
		return glm::quat(glm::radians(mData[1]));
	}

	void FTransform::SetTranslation(const glm::vec3 &translation)
	{
		mData[0] = translation;
		CalculateModelMatrix();
	}

	void FTransform::SetTranslation(float x, float y, float z)
	{
		SetTranslation({x, y, z});
	}

	void FTransform::SetRotation(const glm::vec3 &rotation)
	{
		mData[1] = rotation;
		CalculateModelMatrix();
	}

	void FTransform::SetQuaternion(const glm::quat &quaternion)
	{
		mData[1] = glm::degrees(glm::eulerAngles(quaternion));
		CalculateModelMatrix();
	}

	void FTransform::SetScale(const glm::vec3 &scale)
	{
		mData[2] = scale;
		CalculateModelMatrix();
	}

	void FTransform::AddTranslation(const glm::vec3 &translation)
	{
		mData[0] += translation;
		CalculateModelMatrix();
	}

	void FTransform::AddRotation(const glm::vec3 &rotation)
	{
		mData[1] += rotation;
		CalculateModelMatrix();
	}

	glm::vec3 FTransform::GetForward() const
	{
		return glm::normalize(mModelMatrix[2]);
	}

	glm::vec3 FTransform::GetForwardUnnormalized() const
	{
		return -mModelMatrix[2];
	}

	glm::vec3 FTransform::GetRight() const
	{
		return -glm::normalize(mModelMatrix[0]);
	}

	glm::vec3 FTransform::GetUp() const
	{
		return glm::normalize(mModelMatrix[1]);
	}

	FTransform FTransform::Inverse() const
	{
		return FTransform(glm::inverse(mModelMatrix));
	}

	std::string FTransform::ToString() const
	{
		return "{" + std::format("{},{},{} ", glm::to_string(mData[0]), glm::to_string(mData[1]), glm::to_string(mData[2])) + "}";
	}

	FTransform &FTransform::operator=(const FTransform &rhs)
	{
		mData[0] = rhs.mData[0];
		mData[1] = rhs.mData[1];
		mData[2] = rhs.mData[2];

		CalculateModelMatrix();

		return *this;
	}

	FTransform FTransform::operator*(const FTransform &rhs) const
	{
		auto new_matrix = mModelMatrix * rhs.mModelMatrix;

		return FTransform(new_matrix);
	}

	FTransform FTransform::operator+(const FTransform &rhs) const
	{
		auto t = mData[0] + rhs[0];
		auto r = mData[1] + rhs[1];
		auto s = mData[2] * rhs[2];
		return {t, r, s};
	}

	FTransform &FTransform::operator+=(const FTransform &rhs)
	{
		return *this = *this + rhs;
	}

	FTransform FTransform::operator/(float rhs) const
	{
		auto t = mData[0] / rhs;
		auto r = mData[1] / rhs;
		auto s = mData[2] / rhs;
		return {t, r, s};
	}

	FTransform &FTransform::operator/=(float rhs)
	{
		return *this = *this / rhs;
	}

	bool FTransform::operator==(const FTransform &rhs) const
	{
		return mData == rhs.mData;
	}

	glm::vec3 &FTransform::operator[](int index)
	{
		ASSERT(index >= 0 && index <= 2)
		return mData[index];
	}

	const glm::vec3 &FTransform::operator[](int index) const
	{
		ASSERT(index >= 0 && index <= 2)
		return mData[index];
	}

	void FTransform::CalculateModelMatrix()
	{
		auto rotation = glm::quat(glm::radians(mData[1]));
		mModelMatrix = glm::translate(mData[0]) * glm::toMat4(rotation) * glm::scale(mData[2]);
	}

} // namespace BHive
