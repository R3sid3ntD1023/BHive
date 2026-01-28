#include "AnimationState.h"
#include "Animator/anim_player/AnimPlayerContext.h"

namespace BHive
{
	AnimationState::AnimationState(const Ref<AnimationNodePoseBase> &pose_node)
		: mPoseNode(pose_node)
	{
	}

	void AnimationState::SetPoseNode(const Ref<AnimationNodePoseBase> &node)
	{
		mPoseNode = node;
		SetPhaseCopySource(mPoseNode.get());
	}

	void AnimationState::AddTransition(const Ref<AnimationStateTransition> &transition)
	{
		mTransitionNodes.push_back(transition);
	}

	void AnimationState::Update(const AnimPlayerContext &context)
	{
		AnimationNodePoseBase::Update(context);

		mPoseNode->Update(context);
		SetDuration(mPoseNode->GetDuration());
	}

	void AnimationState::ExecuteImpl(const AnimPlayerContext &context, std::any &out_result)
	{
		ASSERT(mPoseNode != nullptr);

		AnimationNodePoseBase::ExecuteImpl(context, out_result);

		out_result = mPoseNode->Execute(context);

		ApplyNextPhase(context);
	}
} // namespace BHive
