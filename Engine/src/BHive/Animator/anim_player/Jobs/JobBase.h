#pragma once

#include "SkeletalPosePool.h"

namespace BHive
{
    class JobQueue;

    class JobBase
    {
    public:
        virtual ~JobBase() = default;

        void Execute(JobQueue& queue)
        {
            mResult = OnExecute(queue);
        }

        const SkeletalPosePool::ptr& GetResult() const { return mResult; }

        SkeletalPosePool::ptr TransferResult() { return std::move(mResult); }

        void ReleaseResult() { mResult.release(); }

    protected:
        virtual  SkeletalPosePool::ptr OnExecute(JobQueue& queue) = 0;

    private:
        SkeletalPosePool::ptr mResult;
    };
}