#pragma once

#include "core/Core.h"

namespace BHive
{

	template <typename T>
	struct TKeyFrame
	{
		T mValue;
		float mTimeStamp;
	};

	using Vec3Key = TKeyFrame<glm::vec3>;
	using QuatKey = TKeyFrame<glm::quat>;

	struct FrameData
	{
		std::vector<Vec3Key> mPositions;
		std::vector<QuatKey> mRotations;
		std::vector<Vec3Key> mScales;
	};

	template <typename A>
	inline void Save(A &ar, const FrameData &obj)
	{
		ar(obj.mPositions.size(), obj.mRotations.size(), obj.mScales.size());
		ar(MAKE_BINARY(obj.mPositions.data(), obj.mPositions.size() * sizeof(Vec3Key)));
		ar(MAKE_BINARY(obj.mRotations.data(), obj.mRotations.size() * sizeof(QuatKey)));
		ar(MAKE_BINARY(obj.mScales.data(), obj.mScales.size() * sizeof(Vec3Key)));
	}

	template <typename A>
	inline void Load(A &ar, FrameData &obj)
	{
		size_t size = 0;
		ar(size);
		obj.mPositions.resize(size);

		ar(size);
		obj.mRotations.resize(size);

		ar(size);
		obj.mScales.resize(size);

		ar(MAKE_BINARY(const_cast<Vec3Key *>(obj.mPositions.data()), obj.mPositions.size() * sizeof(Vec3Key)));
		ar(MAKE_BINARY(const_cast<QuatKey *>(obj.mRotations.data()), obj.mRotations.size() * sizeof(QuatKey)));
		ar(MAKE_BINARY(const_cast<Vec3Key *>(obj.mScales.data()), obj.mScales.size() * sizeof(Vec3Key)));
	}

} // namespace BHive
