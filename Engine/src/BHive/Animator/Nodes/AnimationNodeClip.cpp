#include "AnimationNodeClip.h"
#include "Animator/AnimatorContext.h"
#include "mesh/AnimationClip.h"

namespace BHive
{
	void AnimationClipNode::ExecuteImpl(const AnimatorContext &context, std::any &out_result)
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

	void AnimationClipNode::Update(const AnimatorContext &context)
	{
		AnimationNodePoseBase::Update(context);

		if (IsFirstPlay(context))
		{
			mClip->PlayFromStart();
		}
	}
} // namespace BHive
