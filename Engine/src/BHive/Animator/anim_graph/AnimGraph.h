#pragma once

#include "Animator/anim_player/BlackBoard.h"
#include "asset/Asset.h"
#include "mesh/SkeletalPose.h"
#include "mesh/Skeleton.h"

namespace BHive
{
	class AnimGraphNodeBase;
	class Animator;
	class Skeleton;

	class BHIVE_API AnimGraph : public Asset
	{
	public:
		using Nodes = std::unordered_map<UUID, Ref<AnimGraphNodeBase>>;

	public:
		AnimGraph() = default;
		AnimGraph(const Ref<Skeleton> &skeleton);

		void add_node(const Ref<AnimGraphNodeBase> &node);

		void remove_node(const UUID &node_id);

		virtual void Save(cereal::BinaryOutputArchive &ar) const;

		virtual void Load(cereal::BinaryInputArchive &ar);

		const Nodes &get_nodes() { return mNodes; }

		const Ref<Skeleton> &get_skeleton() const { return mSkeleton; }

		REFLECTABLEV(Asset)

	private:
		Nodes mNodes;

		Ref<Skeleton> mSkeleton;

		BlackBoard mBlackBoard;
	};

} // namespace BHive