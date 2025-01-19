#include "mesh/SkeletalPose.h"
#include "SkeletalPosePool.h"

namespace BHive
{
	SkeletalPosePool::SkeletalPosePool(const Skeleton *skeleton)
		: mSkeleton(skeleton)
	{
	}

	SkeletalPosePool::~SkeletalPosePool()
	{
		ASSERT(mBorrows == 0);
	}

	SkeletalPosePool::ptr SkeletalPosePool::Borrow()
	{
		ASSERT(mBorrows >= 0);
		mBorrows++;

		if (mPoses.empty())
		{
			return ptr{new SkeletalPose{mSkeleton}, deleter{*this}};
		}

		SkeletalPose *result = mPoses.back().get();
		mPoses.pop_back();
		return ptr{result};
	}

	SkeletalPosePool::deleter::deleter(SkeletalPosePool &pool)
		: mPool(&pool)
	{
	}

	void SkeletalPosePool::deleter::operator()(SkeletalPose *ptr)
	{
		ASSERT(mPool != nullptr);

		ASSERT(mPool->mBorrows > 0);
		mPool->mBorrows--;

		ASSERT(
			std::find_if(
				mPool->mPoses.begin(), mPool->mPoses.end(),
				[ptr](auto &pose) { return pose.get() == ptr; }) == mPool->mPoses.end());

		mPool->mPoses.emplace_back(ptr);
	}
} // namespace BHive