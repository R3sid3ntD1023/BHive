#pragma once

#include "AnimationNodePoseBase.h"

namespace BHive
{

    class AnimationNodeBlend : public AnimationNodePoseBase
    {
    public:
        AnimationNodeBlend() = default;

        void CollectDesendents(std::vector<Ref<AnimationNodeBase>>& children) const override;

        struct PoseData
        {
            Ref<AnimationNodePoseBase> mPoseNode;
            float mFentity{0.0f};
        };

        void Update(const AnimatorContext& context);

        void AddPoseNode(const Ref<AnimationNodePoseBase>& node, float fentity = 0.0f);

        void SetFentityNode(const Ref<AnimationNodePoseBase>& node);

    protected:
        void ExecuteImpl(const AnimatorContext& context, std::any& out_result);

    private:
        void SelectBlendedNodes(const AnimatorContext& context);

    private:
        std::vector<PoseData> mPoseData;
        Ref<AnimationNodePoseBase> mFentityNode{nullptr};
        Ref<AnimationNodePoseBase> mSourceNode{nullptr};
        Ref<AnimationNodePoseBase> mDestinationNode{nullptr};
        
        JobBlend mJobBlend;

        std::optional<float> mPrevFentity;
        float mDestinationNodeWeight{0.0f};
    };
} // namespace BHive
