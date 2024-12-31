#pragma once

#include "JobBase.h"
#include "JobQueue.h"

namespace BHive
{
    class JobSave : public JobBase
    {
    public:
        JobSave() = default;

        void SetSavedPoseIndex(uint64_t index)
        {
            mPoseIndex = index;
        }

        void SetSavedJobIndex(uint64_t index)
        {
            mJobIndex = index;
        }

    protected:
        virtual  SkeletalPosePool::ptr OnExecute(JobQueue& queue)
        {
            queue.SavePose(queue.GetJob(mJobIndex.value()), mPoseIndex.value());
            return nullptr;
        }

        std::optional<uint64_t> mPoseIndex;
        std::optional<uint64_t> mJobIndex;
    };
} // namespace BHive
