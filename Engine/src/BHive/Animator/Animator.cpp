#include "Animator.h"
#include "AnimatorContext.h"
#include "mesh/SkeletalPose.h"
#include "mesh/Skeleton.h"
#include "Nodes/AnimationNodeBase.h"

namespace BHive
{
	Animator::Animator(const Skeleton *skeleton)
		: mQueue(skeleton)
	{
	}

	void Animator::Update(float dt, SkeletalPose &out_pose)
	{
		if (!mRoot)
			return;

		mPlayCounter++;

		AnimatorContext anim_context{
			.mJobQueue = mQueue,
			.mBlackBoard = mBlackBoard,
			.mDeltaTime = dt,
			.mPlayCounter = mPlayCounter};

		mRoot->Execute(anim_context);

		mQueue.Execute(out_pose);
		out_pose.RecalulateObjectSpaceTransforms();
	}

	void Animator::SetRootNode(const Ref<AnimationNodeBase> &node)
	{
		mRoot = node;
	}

	void Animator::AddNode(const Ref<AnimationNodeBase> &node)
	{
		mNodes.push_back(node);
	}

	void Animator::SetBlackBoard(const BlackBoard &blackboard)
	{
		mBlackBoard = blackboard;
	}

} // namespace BHive