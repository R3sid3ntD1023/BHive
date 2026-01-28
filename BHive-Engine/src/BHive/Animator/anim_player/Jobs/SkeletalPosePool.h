#pragma once

#include "core/Core.h"
#include "mesh/SkeletalPose.h"

namespace BHive
{
    class SkeletalPosePool
    {
    public:
        struct deleter final
        {
            explicit deleter() = default;
            explicit deleter(SkeletalPosePool& pool);

            void operator()(SkeletalPose* ptr);

        private:
            SkeletalPosePool* mPool{nullptr};
        };

        using ptr = Scope<SkeletalPose, deleter>;

        SkeletalPosePool(const Skeleton* skeleton);
        ~SkeletalPosePool();

        SkeletalPosePool(const SkeletalPosePool&) = delete;
        SkeletalPosePool(SkeletalPosePool&&) = delete;

        SkeletalPosePool& operator=(const SkeletalPosePool&) = delete;
        SkeletalPosePool& operator=(SkeletalPosePool&&) = delete;
    
        ptr Borrow();

    private:
        const Skeleton* mSkeleton;  
        std::vector<Ref<SkeletalPose>> mPoses;
        uint32_t mBorrows{0};
    };

} // namespace Bhive
