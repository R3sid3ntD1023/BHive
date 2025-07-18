#pragma once

#include "core/Core.h"
#include "Jobs/JobQueue.h"

namespace BHive
{
	class BlackBoard;

	struct AnimPlayerContext
	{
		JobQueue &mJobQueue;

		BlackBoard &mBlackBoard;

		const float mDeltaTime{0.f};

		bool mSyncEnabled{false};

		std::optional<float> mSyncPhase;

		int32_t mPlayCounter{0};
	};
} // namespace BHive
