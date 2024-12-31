#pragma once

#include "core/Core.h"
#include "Jobs/JobQueue.h"


namespace BHive
{
    class BlackBoard;

    struct AnimatorContext
    {
        JobQueue& mJobQueue;

        Ref<BlackBoard> mBlackBoard;

        const float mDeltaTime{0.f};

        bool mSyncEnabled{false};

        std::optional<float> mSyncPhase;

        int32_t mPlayCounter{0};
    };
} // namespace BHive
