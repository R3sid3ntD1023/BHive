#pragma once

#include "Animator/BlackBoard.h"
#include "asset/Asset.h"
#include "mesh/SkeletalPose.h"

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

		void Initialize();

		void Play(float dt);

		virtual void Save(cereal::BinaryOutputArchive &ar) const;

		virtual void Load(cereal::BinaryInputArchive &ar);

		const Nodes &GetNodes() { return mNodes; }

		REFLECTABLEV(Asset)

	private:
		Nodes mNodes;

		Ref<Skeleton> mSkeleton;

		Ref<Animator> mAnimator{nullptr};

		Ref<SkeletalPose> mPose;

		BlackBoard mBlackBoard;
	};

	REFLECT(AnimGraph)
	{
		BEGIN_REFLECT(AnimGraph)
		REFLECT_PROPERTY("BlackBoard", mBlackBoard);
	}
} // namespace BHive