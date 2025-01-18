#include "AnimationNodeBase.h"
#include "Animator/AnimatorContext.h"

namespace BHive
{

	std::any AnimationNodeBase::Execute(const AnimatorContext &context)
	{
		std::any result;
		ExecuteImpl(context, result);

		return result;
	}

	void AnimationNodeBase::ExecuteImpl(const AnimatorContext &context, std::any &out_result)
	{
		RegisterPlay(context);
	}

	bool AnimationNodeBase::IsFirstPlay(const AnimatorContext &context) const
	{
		RegisterPlay(context);
		return mIsFirstPlay;
	}

	void AnimationNodeBase::RegisterPlay(const AnimatorContext &context) const
	{
		if (mLastGraphPlayCounter != context.mPlayCounter)
		{
			mIsFirstPlay =
				(!mLastGraphPlayCounter.has_value() ||
				 (context.mPlayCounter != mLastGraphPlayCounter.value() + 1));
			mLastGraphPlayCounter = context.mPlayCounter;
		}
	}

} // namespace BHive
