#include "JobQueue.h"
#include "JobBase.h"

 namespace BHive
    {
        JobQueue::JobQueue(const Skeleton* skeleton)
            :mPosePool(skeleton)
        {

        }

        uint64_t JobQueue::AddJob(JobBase &job)
        {
            mJobs.push_back(&job);
            return mJobs.size() - 1;
        }

        void JobQueue::SavePose(const JobBase &job, uint64_t pose_index)
        {
            if(mSavedPoses[pose_index] == nullptr)
                mSavedPoses[pose_index] = mPosePool.Borrow();

            *mSavedPoses[pose_index] = *job.GetResult();
        }

        const SkeletalPosePool::ptr &JobQueue::RestorePose(uint64_t pose_index)
        {
            return mSavedPoses[pose_index];
        }

        void JobQueue::Execute(SkeletalPose &out_pose)
        {
            ASSERT(!mJobs.empty());

            JobBase* final {nullptr};

            for(auto* job : mJobs)
            {
                job->Execute(*this);

                if(job->GetResult() != nullptr)
                {
                    final = job;
                }
            }

            out_pose = *final->GetResult();

            for(auto& job : mJobs)
                job->ReleaseResult();

            mJobs.clear();
        }

        JobBase &JobQueue::GetJob(uint64_t index)
        {
            ASSERT(!mJobs.empty());
            return *mJobs.at(index);
        }

        uint64_t JobQueue::AcquireSavePoseSlot()
        {
            mSavedPoses.emplace_back();
            return mSavedPoses.size() - 1;
        }

    } // namespace BHive
