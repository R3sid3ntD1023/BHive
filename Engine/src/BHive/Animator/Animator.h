#pragma once

#include "asset/Asset.h"
#include "BlackBoard.h"
#include "core/Core.h"
#include "core/UUID.h"
#include "Jobs/JobQueue.h"

namespace BHive
{
	class SkeletalPose;
	class Skeleton;
	class AnimationNodeBase;
	class BlackBoard;

	class Animator
	{
	public:
		Animator() = default;
		Animator(const Skeleton *skeleton);

		void Update(float dt, SkeletalPose &out_pose);

		void SetRootNode(const Ref<AnimationNodeBase> &node);

		void AddNode(const Ref<AnimationNodeBase> &node);

		void SetBlackBoard(const BlackBoard &blackboard);

		const BlackBoard &GetBlackBoard() const { return mBlackBoard; }

	private:
		JobQueue mQueue;
		std::vector<Ref<AnimationNodeBase>> mNodes;
		BlackBoard mBlackBoard;
		Ref<AnimationNodeBase> mRoot;
		int32_t mPlayCounter{0};
	};

} // namespace BHive
