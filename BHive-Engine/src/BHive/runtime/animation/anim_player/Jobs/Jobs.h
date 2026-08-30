#pragma once

#include "JobBase.h"
#include "JobQueue.h"
#include "gfx/animation/AnimationClip.h"

namespace BHive
{
	class AnimationClip;

	class JobClip : public JobBase
	{
	public:
		JobClip() = default;

		void SetClip(AnimationClip &clip) { mClip = &clip; }

		void SetSkeleton(Skeleton *skeleton) { mSkeleton = skeleton; }

		void SetTime(float time)
		{
			ASSERT(mClip);
			ASSERT(time >= 0.0f && time <= mClip->GetDuration());

			mTime = time;
		}

	protected:
		virtual SkeletalPosePool::ptr OnExecute(JobQueue &queue)
		{
			auto pose_ptr{queue.GetPosePool().Borrow()};

			if (mSkeleton && mClip)
			{
				mClip->Play(mTime, *pose_ptr, mSkeleton);
			}

			return pose_ptr;
		}

	private:
		AnimationClip *mClip{nullptr};
		Skeleton *mSkeleton = nullptr;
		float mTime{0.0f};
	};

	class JobBlend : public JobBase
	{
	public:
		JobBlend() = default;

		float GetWeight() const { return mCurrentWeight; }

		void SetWeight(float weight) { mCurrentWeight = weight; }

		void SetFirstJob(uint64_t index) { mFirstJob = index; }

		void SetSecondJob(uint64_t index) { mSecondJob = index; }

	protected:
		SkeletalPosePool::ptr OnExecute(JobQueue &queue)
		{
			auto &first_job = queue.GetJob(mFirstJob);
			const auto &second_job = queue.GetJob(mSecondJob);

			auto p0{first_job.TransferResult()};
			const auto &p1{second_job.GetResult()};

			SkeletalPoseBlend(*p0, *p1, mCurrentWeight, *p0);

			return p0;
		}

	private:
		float mCurrentWeight = 0.0f;
		uint64_t mFirstJob;
		uint64_t mSecondJob;
	};
} // namespace BHive