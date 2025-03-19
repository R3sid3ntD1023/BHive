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

	class AnimGraph : public Asset
	{
	public:
		using Nodes = std::unordered_map<UUID, Ref<AnimGraphNodeBase>>;

	public:
		AnimGraph() = default;
		AnimGraph(const Ref<Skeleton> &skeleton);

		virtual void Save(cereal::BinaryOutputArchive &ar) const;

		virtual void Load(cereal::BinaryInputArchive &ar);

		const Nodes &GetNodes() { return mNodes; }

		const Ref<Skeleton> &GetSkeleton() const { return mSkeleton; }

		REFLECTABLEV(Asset)

	private:
		Nodes mNodes;

		Ref<Skeleton> mSkeleton;

		BlackBoard mBlackBoard;
	};

} // namespace BHive