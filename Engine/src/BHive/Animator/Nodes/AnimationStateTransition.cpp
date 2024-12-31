#include "AnimationStateTransition.h"
#include "AnimationStateMachine.h"
#include "AnimationState.h"
#include "Animator/AnimatorContext.h"

namespace BHive
{
    AnimationStateTransition::AnimationStateTransition(const float duration, bool reversible)
        :mReversible(reversible)
    {
        SetPhaseRules(0);

        SetDuration(duration);
    }

    void AnimationStateTransition::Update(const AnimatorContext &context)
    {
        AnimationNodePoseBase::Update(context);

        if(IsFirstPlay(context))
        {
            ASSERT(AreConditionsSatisfied(context));

            mReversed = false;

            mSavedPoseSlotIndex = 0;
            mSavedPoseSourcePhase = 0.f;
        }  

        bool switch_reversed_status{false};
        if(mReversible)
        {
            const bool can_continue = AreConditionsSatisfied(context);
            switch_reversed_status = (mReversed && can_continue) || (!mReversed && !can_continue);
        }

        if(switch_reversed_status)
        {
            mReversed = !mReversed;

            mSavedPoseSlotIndex = (mSavedPoseSlotIndex == 0) ? 1 : 0;
            mSavedPoseSourcePhase = GetPhase();
        }

        SetPhaseRules(EPhaseRules::REVSERSED, mReversed);

        const auto state_machine = AnimationStateMachine::GetCurrent();
        ASSERT(state_machine);

        if(mReversed)
        {
            mCurrentSource = mDestinationStateNode;
            mCurrentDestination = state_machine->GetTransitionSource();
        }
        else{
            mCurrentSource = state_machine->GetTransitionSource();
            mCurrentDestination = mDestinationStateNode;
        }

        mCurrentDestination->Update(context);
    }

    bool AnimationStateTransition::AreConditionsSatisfied(const AnimatorContext &context) const
    {
        return std::any_cast<bool>(mConditionNode->Execute(context));
    }

    bool AnimationStateTransition::IsFinished(float phase)
    {
        if(mReversed)
            return phase <= 0.f;

        return phase >= 1.f;
    }

    void AnimationStateTransition::SetConditonNode(const Ref<AnimationNodeBase> &node)
    {
        mConditionNode = node;
    }

    void AnimationStateTransition::SetDestinationStateNode(const Ref<AnimationState> &node)
    {
        mDestinationStateNode = node;
    }

    void AnimationStateTransition::SetReversible(bool reversible)
    {
        mReversible = reversible;
    }

    void AnimationStateTransition::ExecuteImpl(const AnimatorContext & context, std::any & out_result)
    {
        AnimationNodePoseBase::ExecuteImpl(context, out_result);
        
        ApplyNextPhase(context);

        if(!mSavedPoseSlotsAcquired)
        {
            mSavedPoseSlotsAcquired = true;
            for(size_t i = 0; i < mSavedPoseSlots.size(); i++)
            {
                mSavedPoseSlots.at(i) = context.mJobQueue.AcquireSavePoseSlot();
            }
        }

        const uint64_t saved_pose_source_slot = mSavedPoseSlots.at(mSavedPoseSlotIndex);
        const uint64_t saved_pose_transition_slot = mSavedPoseSlotIndex == 0 ? mSavedPoseSlots[1] : mSavedPoseSlots[0];

        const auto state_machine = AnimationStateMachine::GetCurrent();
        ASSERT(state_machine);

        if(IsFirstPlay(context))
        {
            AnimatorContext context_pass_on{context};

            context_pass_on.mSyncEnabled = true;
            context_pass_on.mSyncPhase = std::clamp(state_machine->GetTransitionSourcePhase(), 0.0f, 1.f);

            const auto source_job = std::any_cast<uint64_t>(mCurrentSource->Execute(context_pass_on));

            mSaveSourceJob.SetSavedJobIndex(source_job);
            mSaveSourceJob.SetSavedPoseIndex(saved_pose_source_slot);
            context.mJobQueue.AddJob(mSaveSourceJob);
        }

        mRestoreJob.SetSavedPoseIndex(saved_pose_source_slot);
        const uint64_t restore_job_index = context.mJobQueue.AddJob(mRestoreJob);
        

        const uint64_t destination_job_index = std::any_cast<uint64_t>(mCurrentDestination->Execute(context));

        mBlendJob.SetFirstJob(restore_job_index);
        mBlendJob.SetSecondJob(destination_job_index);

        const float blend_phase_current = GetPhase();
        const float blend_phase_from = mSavedPoseSourcePhase;
        const float blend_phase_duration = mReversed ? mSavedPoseSourcePhase : 1.0f - mSavedPoseSourcePhase;

        if(blend_phase_duration == 0.f)
        {
            mBlendJob.SetWeight(1.f);
        }
        else{
            mBlendJob.SetWeight(std::abs(blend_phase_current - blend_phase_from) / blend_phase_duration);
        }

        const uint64_t blend_job_index = context.mJobQueue.AddJob(mBlendJob);

        mSaveTransitionJob.SetSavedJobIndex(blend_job_index);
        mSaveTransitionJob.SetSavedPoseIndex(saved_pose_transition_slot);
        context.mJobQueue.AddJob(mSaveTransitionJob);

        out_result = blend_job_index;
    }
}