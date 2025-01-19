#include "AnimationNodeClip.h"
#include "Animator/anim_player/AnimPlayerContext.h"
#include "mesh/AnimationClip.h"

namespace BHive
{
	void AnimationClipNode::ExecuteImpl(const AnimPlayerContext &context, std::any &out_result)
	{
		AnimationNodePoseBase::ExecuteImpl(context, out_result);

		ApplyNextPhase(context);

		mJobClip.SetTime(context.mDeltaTime);
		out_result = context.mJobQueue.AddJob(mJobClip);
	}

	void AnimationClipNode::SetAnimationClip(const Ref<AnimationClip> &clip)
	{
		mClip = clip;
		mJobClip.SetClip(*mClip);
		SetDuration(mClip->GetLengthInSeconds());
	}

	void AnimationClipNode::Update(const AnimPlayerContext &context)
	{
		AnimationNodePoseBase::Update(context);

		if (IsFirstPlay(context))
		{
			mClip->PlayFromStart();
		}
	}
} // namespace BHive
