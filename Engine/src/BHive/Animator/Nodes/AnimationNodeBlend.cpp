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

        if(mFactorNode)
        {
            children.push_back(mFactorNode);
            mFactorNode->CollectDesendents(children);
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

    void AnimationNodeBlend::AddPoseNode(const Ref<AnimationNodePoseBase> &node, float factor)
    {
        mPoseData.push_back({node, factor});
    }

    void AnimationNodeBlend::SetFactorNode(const Ref<AnimationNodePoseBase> &node)
    {
        mFactorNode = node;
    }

    void AnimationNodeBlend::SelectBlendedNodes(const AnimatorContext & context)
    {
        ASSERT(!mPoseData.empty());
        ASSERT(mFactorNode != nullptr);
        ASSERT(std::is_sorted(mPoseData.begin(), mPoseData.end(), [](const auto& p0, const auto& p1){
            return p0.mFactor < p1.mFactor;
        }));

        float factor = mFactorNode ? std::any_cast<float>(mFactorNode->Execute(context)) : 0.0f;

        if(factor == mPrevFactor)
            return;

        mPrevFactor = factor;

        size_t child_count = mPoseData.size();

        std::optional<int32_t> upper_bound_child_index_opt;
        for(size_t i = 0; i < child_count; i++)
        {
            if(mPoseData[i].mFactor >= factor)
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

            if(upper_bound_node.mFactor == factor)
            {
                mSourceNode = nullptr;
                mDestinationNode = upper_bound_node.mPoseNode;
                mDestinationNodeWeight = 1.f;
            }
            else
            {
                ASSERT(upper_bound_child_index >= 1);
                PoseData& lower_bound_node = mPoseData[upper_bound_child_index - 1];

                const float factor_range = upper_bound_node.mFactor - lower_bound_node.mFactor;
                ASSERT(factor_range != 0.0f);

                mSourceNode = lower_bound_node.mPoseNode;
                mDestinationNode = upper_bound_node.mPoseNode;
                mDestinationNodeWeight = (factor - lower_bound_node.mFactor) / factor_range;
                ASSERT(mDestinationNodeWeight >= 0.f && mDestinationNodeWeight <= 1.0f); 
            }
        }
    }

}