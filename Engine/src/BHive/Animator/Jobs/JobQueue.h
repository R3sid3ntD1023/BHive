#pragma once

#include "SkeletalPosePool.h"

namespace BHive
{
    class JobBase;

    class JobQueue
    {
    public:
        JobQueue(const Skeleton* skeleton);

        uint64_t AddJob(JobBase& job);

        void SavePose(const JobBase& job, uint64_t pose_index);

        const SkeletalPosePool::ptr& RestorePose(uint64_t pose_index);

        void Execute(SkeletalPose& out_pose);

        JobBase& GetJob(uint64_t index);

        SkeletalPosePool& GetPosePool() { return mPosePool; }
        
        uint64_t AcquireSavePoseSlot();

    private:
        std::vector<JobBase*> mJobs;
        std::vector<SkeletalPosePool::ptr> mSavedPoses;
        SkeletalPosePool mPosePool;
    };
}