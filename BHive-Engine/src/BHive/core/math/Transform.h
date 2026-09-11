#pragma once

#include "core/Core.h"

namespace BHive
{
	struct BHIVE_API FTransform
	{
		glm::vec3 Translation = {0, 0, 0};
		glm::vec3 Rotation = {0, 0, 0};
		glm::vec3 Scale = {1, 1, 1};

		FTransform Rotate(const glm::vec3 &eulerDegrees) const;

		glm::vec3 TransformPoint(const glm::vec3 &p) const;

		glm::vec3 GetForward() const;

		glm::vec3 GetForwardUnnormalized() const;

		glm::vec3 GetRight() const;

		glm::vec3 GetUp() const;

		FTransform Inverse() const;

		FTransform &FromMat(const glm::mat4 &m);

		glm::mat4 ToMat4() const;

		glm::quat Quaternion() const;

		std::string ToString() const;

		FTransform operator*(const FTransform &rhs) const;

		bool operator==(const FTransform &rhs) const;

		template <typename A>
		inline void Save(A &ar) const
		{
			ar(Translation, Rotation, Scale);
		}

		template <typename A>
		inline void Load(A &ar)
		{
			ar(Translation, Rotation, Scale);
		}
	};

	template <typename Ostream>
	inline Ostream &operator<<(Ostream &os, const FTransform &obj)
	{
		os << '{' << obj.Translation << ',' << obj.Rotation << ',' << obj.Scale << '}';
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
