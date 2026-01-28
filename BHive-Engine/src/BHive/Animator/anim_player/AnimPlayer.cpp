#include "Animator/anim_graph/AnimGraph.h"
#include "AnimPlayer.h"
#include "AnimPlayerContext.h"
#include "mesh/SkeletalPose.h"
#include "mesh/Skeleton.h"
#include "Nodes/AnimationNodeBase.h"

namespace BHive
{
	AnimPlayer::AnimPlayer(const AnimGraph &graph)
		: mQueue(graph.get_skeleton().get())
	{
	}

	void AnimPlayer::Update(float dt, SkeletalPose &out_pose)
	{
		if (!mRoot)
			return;

		mPlayCounter++;

		AnimPlayerContext anim_context{
			.mJobQueue = mQueue, .mBlackBoard = mBlackBoard, .mDeltaTime = dt, .mPlayCounter = mPlayCounter};

		mRoot->Execute(anim_context);

		mQueue.Execute(out_pose);
		out_pose.RecalulateObjectSpaceTransforms();
	}

} // namespace BHive