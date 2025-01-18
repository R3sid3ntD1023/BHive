#include "Animator/Animator.h"
#include "AnimGraph.h"
#include "nodes/AnimGraphNodeBase.h"

namespace BHive
{
	AnimGraph::AnimGraph(const Ref<Skeleton> &skeleton)
		: mSkeleton(skeleton)
	{
	}
	void AnimGraph::Initialize()
	{
		mAnimator = CreateRef<Animator>(mSkeleton.get().get());
		mAnimator->SetBlackBoard(mBlackBoard);
		mPose = CreateRef<SkeletalPose>(mSkeleton.get().get());
	}
	void AnimGraph::Play(float dt)
	{
		if (mAnimator)
		{
			mAnimator->Update(dt, *mPose);
		}
	}

	void AnimGraph::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(mSkeleton, mBlackBoard, mNodes.size());

		for (const auto &[id, node] : mNodes)
		{
			ar(id, node->get_type());
			node->Serialize(ar);
		}
	}

	void AnimGraph::Load(cereal::BinaryInputArchive &ar)
	{
		size_t num_nodes = 0;
		ar(mSkeleton, mBlackBoard, num_nodes);

		for (size_t i{}; i < num_nodes; i++)
		{
			UUID node_id = 0;
			rttr::type node_type = InvalidType;

			ar(node_id, node_type);

			auto node = node_type.create({this}).get_value<Ref<AnimGraphNodeBase>>();

			if (node)
			{
				node->Serialize(ar);
			}
		}

		Initialize();
	}

	REFLECT(AnimGraph)
	{
		BEGIN_REFLECT(AnimGraph)
		REFLECT_CONSTRUCTOR()
		REFLECT_PROPERTY_READ_ONLY("Skeleton", mSkeleton)
		REFLECT_PROPERTY("BlackBoard", mBlackBoard);
	}
} // namespace BHive