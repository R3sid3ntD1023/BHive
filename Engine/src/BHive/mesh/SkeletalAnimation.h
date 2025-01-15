#pragma once

#include "core/Core.h"
#include "math/Math.h"
#include "AnimationFrames.h"
#include "asset/Asset.h"

namespace BHive
{
	class Skeleton;

	typedef std::map<std::string, FrameData> Frames;

	class SkeletalAnimation : public Asset
	{
	public:
		SkeletalAnimation() = default;
		SkeletalAnimation(
			float duration, float ticksPerSecond, const Frames &frames, Ref<Skeleton> skeleton,
			const glm::mat4 &globalInverseMatrix);

		bool Contains(const std::string &name) const;

		Ref<Skeleton> GetSkeleton() { return mSkeleton; }

		float GetTicksPerSecond() const { return mTicksPerSecond; }
		float GetDuration() const { return mDuration; }
		float GetLengthInSeconds() const { return mDuration / mTicksPerSecond; }

		const glm::mat4 &GetGlobalInverseTransformation() { return mGlobalInverseTransformation; }
		float CalculateAnimationTimeTicks(float time);

		glm::vec3 InterpolatePosition(const std::string &name, float animationTime);
		glm::quat InterpolateRotation(const std::string &name, float animationTime);
		glm::vec3 InterpolateScaling(const std::string &name, float animationTime);

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(Asset)

	private:
		int32_t GetPositionIndex(const std::string &name, float aniamtionTime);
		int32_t GetRotationIndex(const std::string &name, float aniamtionTime);
		int32_t GetScaleIndex(const std::string &name, float aniamtionTime);
		float GetScaleFentity(float lastTimeStamp, float nextTimeStamp, float animationTime);

	private:
		float mDuration;
		float mTicksPerSecond;
		glm::mat4 mGlobalInverseTransformation;
		Frames mFrameData;
		Ref<Skeleton> mSkeleton;
	};

	REFLECT_EXTERN(SkeletalAnimation)
} // namespace BHive