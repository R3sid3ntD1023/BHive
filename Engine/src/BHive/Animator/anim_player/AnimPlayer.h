#pragma once

#include "asset/Asset.h"
#include "BlackBoard.h"
#include "core/Core.h"
#include "Jobs/JobQueue.h"

namespace BHive
{
	class SkeletalPose;
	class Skeleton;
	class AnimationNodeBase;
	class BlackBoard;
	class AnimGraph;

	class AnimPlayer
	{
	public:
		AnimPlayer(const AnimGraph &graph);

		void Update(float dt, SkeletalPose &out_pose);

	private:
		JobQueue mQueue;
		std::vector<Ref<AnimationNodeBase>> mNodes;
		BlackBoard mBlackBoard;
		Ref<AnimationNodeBase> mRoot;
		int32_t mPlayCounter{0};
	};

} // namespace BHive
