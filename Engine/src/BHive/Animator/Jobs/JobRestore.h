#pragma once

#include "JobBase.h"
#include "JobQueue.h"

namespace BHive
{
    class JobRestore : public JobBase
    {
    public:
        JobRestore() = default;

        void SetSavedPoseIndex(uint64_t index)
        {
            mPoseIndex = index;
        }

    protected:
        virtual  SkeletalPosePool::ptr OnExecute(JobQueue& queue)
        {
            auto& restored_pose{queue.RestorePose(mPoseIndex.value())};
            ASSERT(restored_pose != nullptr);

            SkeletalPosePool::ptr pose_ptr{queue.GetPosePool().Borrow()};
            *pose_ptr = *restored_pose;
            return pose_ptr;
        }

        std::optional<uint64_t> mPoseIndex;
    };
} // namespace BHive
