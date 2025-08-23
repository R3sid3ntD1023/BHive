#pragma once

#include "core/Core.h"

namespace BHive
{
	struct BHIVE_API FTransform
	{
		FTransform();

		FTransform(const glm::mat4 &matrix);

		FTransform(const glm::vec3 &translation, const glm::vec3 &rotation = {0, 0, 0}, const glm::vec3 &scale = {1, 1, 1});

		FTransform(const glm::vec3 &translation, const glm::quat &rotation);

		const glm::vec3 &GetTranslation() const { return mData[0]; }

		const glm::vec3 &GetRotation() const { return mData[1]; }

		const glm::vec3 &GetScale() const { return mData[2]; }

		const glm::quat GetQuaternion() const;

		void SetTranslation(const glm::vec3 &translation);

		void SetTranslation(float x, float y, float z);

		void SetRotation(const glm::vec3 &rotation);

		void SetQuaternion(const glm::quat &quaternion);

		void SetScale(const glm::vec3 &scale);

		void AddTranslation(const glm::vec3 &translation);

		void AddRotation(const glm::vec3 &rotation);

		glm::vec3 GetForward() const;

		glm::vec3 GetForwardUnnormalized() const;

		glm::vec3 GetRight() const;

		glm::vec3 GetUp() const;

		FTransform Inverse() const;

		const glm::mat4 &to_mat4() const { return mModelMatrix; }

		operator const glm::mat4 &() const { return to_mat4(); }

		std::string ToString() const;

		FTransform &operator=(const FTransform &rhs);

		FTransform operator*(const FTransform &rhs) const;

		FTransform operator+(const FTransform &rhs) const;

		FTransform &operator+=(const FTransform &rhs);

		FTransform operator/(float rhs) const;

		FTransform &operator/=(float rhs);

		glm::vec3 &operator[](int index);

		const glm::vec3 &operator[](int index) const;

		bool operator==(const FTransform &rhs) const;

		template <typename A>
		inline void Save(A &ar) const
		{
			ar(mData[0], mData[1], mData[2]);
		}

		template <typename A>
		inline void Load(A &ar)
		{
			ar(mData[0], mData[1], mData[2]);
			CalculateModelMatrix();
		}

	private:
		void CalculateModelMatrix();

	private:
		glm::vec3 mData[3] = {}; // translation, rotation, scale
		glm::mat4 mModelMatrix;
	};

	template <typename Ostream>
	inline Ostream &operator<<(Ostream &os, const FTransform &obj)
	{
		os << '{' << obj.GetTranslation() << ',' << obj.GetRotation() << ',' << obj.GetScale() << '}';
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

} // namespace BHive
