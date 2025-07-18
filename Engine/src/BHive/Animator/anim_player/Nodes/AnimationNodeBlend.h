#pragma once

#include "AnimationNodePoseBase.h"
#include "Animator/anim_player/Jobs/Jobs.h"

namespace BHive
{

	class AnimationNodeBlend : public AnimationNodePoseBase
	{
	public:
		AnimationNodeBlend() = default;

		void CollectDesendents(std::vector<Ref<AnimationNodeBase>> &children) const override;

		struct PoseData
		{
			Ref<AnimationNodePoseBase> mPoseNode;
			float mFactor{0.0f};
		};

		void Update(const AnimPlayerContext &context);

		void AddPoseNode(const Ref<AnimationNodePoseBase> &node, float fentity = 0.0f);

		void SetFactorNode(const Ref<AnimationNodePoseBase> &node);

	protected:
		void ExecuteImpl(const AnimPlayerContext &context, std::any &out_result);

	private:
		void SelectBlendedNodes(const AnimPlayerContext &context);

	private:
		std::vector<PoseData> mPoseData;
		Ref<AnimationNodePoseBase> mFactorNode{nullptr};
		Ref<AnimationNodePoseBase> mSourceNode{nullptr};
		Ref<AnimationNodePoseBase> mDestinationNode{nullptr};

		JobBlend mJobBlend;

		std::optional<float> mPrevFactor;
		float mDestinationNodeWeight{0.0f};
	};
} // namespace BHive
