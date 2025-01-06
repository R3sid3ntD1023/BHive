#include "AnimationNodeBlend.h"
#include "Animator/AnimatorContext.h"

namespace BHive
{
    void AnimationNodeBlend::ExecuteImpl(const AnimatorContext& context, std::any& out_result)
    {
        AnimationNodePoseBase::ExecuteImpl(context, out_result);

        const float phase_unnormalized = GetNextPhaseUnwrapped(context);

        ApplyNextPhase(context);

        AnimatorContext context_pass_on{context};
        context_pass_on.mSyncEnabled = true;
        context_pass_on.mSyncPhase = phase_unnormalized;

        if(mSourceNode == nullptr)
        {
            out_result = mDestinationNode->Execute(context_pass_on);
            return;
        }

        auto j0 = std::any_cast<uint64_t>(mSourceNode->Execute(context_pass_on));
        auto j1 = std::any_cast<uint64_t>(mDestinationNode->Execute(context_pass_on));

        mJobBlend.SetFirstJob(j0);
        mJobBlend.SetSecondJob(j1);
        mJobBlend.SetWeight(mDestinationNodeWeight);

        out_result = context.mJobQueue.AddJob(mJobBlend);
    }

    void AnimationNodeBlend::CollectDesendents(std::vector<Ref<AnimationNodeBase>>& children) const
    {
        for(auto& pose : mPoseData)
        {
            children.push_back(pose.mPoseNode);
            pose.mPoseNode->CollectDesendents(children);
        }

        if(mFentityNode)
        {
            children.push_back(mFentityNode);
            mFentityNode->CollectDesendents(children);
        }
    }

    void AnimationNodeBlend::Update(const AnimatorContext &context)
    {
        AnimationNodePoseBase::Update(context);

        SelectBlendedNodes(context);

        if(mSourceNode == nullptr)
        {
            mDestinationNode->Update(context);
            SetDuration(mDestinationNode->GetDuration());
        }
        else{
            mSourceNode->Update(context);
            mDestinationNode->Update(context);
            SetDuration(std::lerp(mSourceNode->GetDuration(), mDestinationNode->GetDuration(), mDestinationNodeWeight));
        }
    }

    void AnimationNodeBlend::AddPoseNode(const Ref<AnimationNodePoseBase> &node, float fentity)
    {
        mPoseData.push_back({node, fentity});
    }

    void AnimationNodeBlend::SetFentityNode(const Ref<AnimationNodePoseBase> &node)
    {
        mFentityNode = node;
    }

    void AnimationNodeBlend::SelectBlendedNodes(const AnimatorContext & context)
    {
        ASSERT(!mPoseData.empty());
        ASSERT(mFentityNode != nullptr);
        ASSERT(std::is_sorted(mPoseData.begin(), mPoseData.end(), [](const auto& p0, const auto& p1){
            return p0.mFentity < p1.mFentity;
        }));

        float fentity = mFentityNode ? std::any_cast<float>(mFentityNode->Execute(context)) : 0.0f;

        if(fentity == mPrevFentity)
            return;

        mPrevFentity = fentity;

        size_t child_count = mPoseData.size();

        std::optional<int32_t> upper_bound_child_index_opt;
        for(size_t i = 0; i < child_count; i++)
        {
            if(mPoseData[i].mFentity >= fentity)
            {
                upper_bound_child_index_opt = (int32_t)i;
                break;
            }
        }

        if(!upper_bound_child_index_opt.has_value())
        {
            mSourceNode = nullptr;
            mDestinationNode = mPoseData[child_count - 1].mPoseNode;
            mDestinationNodeWeight = 1.0f;
        }
        else
        {
            auto upper_bound_child_index = upper_bound_child_index_opt.value();
            PoseData& upper_bound_node = mPoseData[upper_bound_child_index];

            if(upper_bound_node.mFentity == fentity)
            {
                mSourceNode = nullptr;
                mDestinationNode = upper_bound_node.mPoseNode;
                mDestinationNodeWeight = 1.f;
            }
            else
            {
                ASSERT(upper_bound_child_index >= 1);
                PoseData& lower_bound_node = mPoseData[upper_bound_child_index - 1];

                const float fentity_range = upper_bound_node.mFentity - lower_bound_node.mFentity;
                ASSERT(fentity_range != 0.0f);

                mSourceNode = lower_bound_node.mPoseNode;
                mDestinationNode = upper_bound_node.mPoseNode;
                mDestinationNodeWeight = (fentity - lower_bound_node.mFentity) / fentity_range;
                ASSERT(mDestinationNodeWeight >= 0.f && mDestinationNodeWeight <= 1.0f); 
            }
        }
    }

}