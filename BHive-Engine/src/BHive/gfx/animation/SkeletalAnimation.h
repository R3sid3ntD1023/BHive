#pragma once

#include "AnimationFrames.h"
#include "asset/Asset.h"
#include "core/Core.h"

namespace BHive
{
	class Skeleton;

		class BHIVE_API SkeletalAnimation : public Asset
	{
	public:
		using Frames = std::unordered_map<uint64_t, FrameData>;

		SkeletalAnimation() = default;
		SkeletalAnimation(float duration, float ticksPerSecond, const Frames &frames);

		bool Contains(uint64_t hash) const;

		float GetTicksPerSecond() const { return mTicksPerSecond; }
		float GetDuration() const { return mDuration; }
		float GetLengthInSeconds() const { return mDuration / mTicksPerSecond; }

		float CalculateAnimationTimeTicks(float time);

		glm::vec3 InterpolatePosition(uint64_t hash, float animationTime);
		glm::quat InterpolateRotation(uint64_t hash, float animationTime);
		glm::vec3 InterpolateScaling(uint64_t hash, float animationTime);

		const FrameData *FindFrames(uint64_t hash) const;

		REFLECTABLEV(Asset)

	private:
		int32_t GetPositionIndex(uint64_t hash, float aniamtionTime);
		int32_t GetRotationIndex(uint64_t hash, float aniamtionTime);
		int32_t GetScaleIndex(uint64_t hash, float aniamtionTime);
		float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);

	private:
		float mDuration{};
		float mTicksPerSecond{};
		Frames mFrames;
	};

	REFLECT_EXTERN(SkeletalAnimation)
} // namespace BHive