#include "AnimGraph.h"
#include "nodes/AnimGraphNodeBase.h"

namespace BHive
{
	AnimGraph::AnimGraph(AssetHandle<Skeleton> skeleton)
		: mSkeleton(skeleton)
	{
	}

	void AnimGraph::add_node(const Ref<AnimGraphNodeBase> &node)
	{
		node->mParentGraph = this;
		mNodes.emplace(node->GetID(), node);
	}

	void AnimGraph::remove_node(const UUID &node_id)
	{
		mNodes.erase(node_id);
	}

	void AnimGraph::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);

		ar(mSkeleton, mBlackBoard, mNodes.size());

		for (const auto &[id, node] : mNodes)
		{
			ar(id, node->get_type());
			node->Serialize(ar);
		}
	}

	void AnimGraph::Load(cereal::BinaryInputArchive &ar)
	{
		Asset::Load(ar);

		size_t num_nodes = 0;
		ar(mSkeleton, mBlackBoard, num_nodes);

		for (size_t i{}; i < num_nodes; i++)
		{
			UUID node_id = NullID;
			rttr::type node_type = InvalidType;

			ar(node_id, node_type);

			auto node = node_type.create().get_value<Ref<AnimGraphNodeBase>>();

			if (node)
			{
				node->Serialize(ar);
				add_node(node);
			}
		}
	}

	REFLECT(AnimGraph)
	{
		BEGIN_REFLECT(AnimGraph)
		REFLECT_CONSTRUCTOR()
		REFLECT_PROPERTY_READ_ONLY("Skeleton", mSkeleton)
		REFLECT_PROPERTY("BlackBoard", mBlackBoard);
	}
} // namespace BHive