#include "AnimationNodeBase.h"
#include "Animator/anim_player/AnimPlayerContext.h"

namespace BHive
{

	std::any AnimationNodeBase::Execute(const AnimPlayerContext &context)
	{
		std::any result;
		ExecuteImpl(context, result);

		return result;
	}

	void AnimationNodeBase::ExecuteImpl(const AnimPlayerContext &context, std::any &out_result)
	{
		RegisterPlay(context);
	}

	bool AnimationNodeBase::IsFirstPlay(const AnimPlayerContext &context) const
	{
		RegisterPlay(context);
		return mIsFirstPlay;
	}

	void AnimationNodeBase::RegisterPlay(const AnimPlayerContext &context) const
	{
		if (mLastGraphPlayCounter != context.mPlayCounter)
		{
			mIsFirstPlay = (!mLastGraphPlayCounter.has_value() || (context.mPlayCounter != mLastGraphPlayCounter.value() + 1));
			mLastGraphPlayCounter = context.mPlayCounter;
		}
	}

} // namespace BHive
