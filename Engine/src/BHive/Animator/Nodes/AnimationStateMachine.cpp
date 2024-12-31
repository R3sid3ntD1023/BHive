#include "AnimationStateMachine.h"
#include "AnimationState.h"
#include "AnimationStateTransition.h"
#include "Animator/AnimatorContext.h"

namespace BHive
{
    AnimationStateMachine::AnimationStateMachine()
    {
        SetPhaseRules(EPhaseRules::COPY);
    }

    const AnimationStateMachine *AnimationStateMachine::GetCurrent()
    {
        ASSERT(!sActiveStateMachines.empty());
        return sActiveStateMachines.top();
    }

    void AnimationStateMachine::Update(const AnimatorContext &context)
    {
        AnimationNodePoseBase::Update(context);

        sActiveStateMachines.push(this);

        if(IsFirstPlay(context))
            mCurrentNode = mNodes[0];

        const bool predict_phase = (Cast<AnimationStateTransition>(mCurrentNode) != nullptr)|| context.mSyncEnabled;

        if(predict_phase)
        {
            mCurrentNode->Update(context);

            const float next_phase = mCurrentNode->GetNextPhaseUnwrapped(context);

            if(UpdateState(context, next_phase))
            {
                mCurrentNode->Update(context);
            }
        }
        else{
            UpdateState(context, GetPhase());
            mCurrentNode->Update(context);
        }

        if(auto transition = Cast<AnimationStateTransition>(mCurrentNode))
        {
            const auto current_destination = transition->GetCurrentDestinationState();

            SetDuration(current_destination->GetDuration());
        }
        else{
            SetDuration(mCurrentNode->GetDuration());
        }

        UpdatePhaseCopySource();

        sActiveStateMachines.pop();
    }

    void AnimationStateMachine::AddState(const Ref<AnimationState> &node)
    {
        mNodes.push_back(node);
    }

    void AnimationStateMachine::ExecuteImpl(const AnimatorContext & context, std::any & out_result)
    {
        AnimationNodePoseBase::ExecuteImpl(context, out_result);

        sActiveStateMachines.push(this);

        if(context.mSyncEnabled)
        {
            if(auto state = Cast<AnimationState>(mCurrentNode))
            {
                //UpdateState(context, GetPhase());
            }
        }

        out_result = mCurrentNode->Execute(context);

        UpdatePhaseCopySource();
        ApplyNextPhase(context);

        sActiveStateMachines.pop();
    }
    
    bool AnimationStateMachine::UpdateState(const AnimatorContext &context, float pending_phase)
    {
        if(auto state = Cast<AnimationState>(mCurrentNode))
        {
            const auto& transitions = state->GetTransitions();

            mTransitionSourceCandidate = state;

            // for(const auto& transition : transitions)
            // {
            //     if(!transition->AreConditionsSatisfied(context))
            //         continue;

            //     mTransitionSource = state;
            //     mCurrentNode = transition;
            //     return true;
            // }

            mTransitionSourceCandidatePhase = pending_phase;

            for(const auto& transition : transitions)
            {
                if(!transition->AreConditionsSatisfied(context))
                    continue;

                mTransitionSource = state;
                mCurrentNode = transition;
                return true;     
            }
        }
        else
        {
            if( auto transition = Cast<AnimationStateTransition>(mCurrentNode))
            {
                if(transition->IsFinished(pending_phase))
                {
                    mCurrentNode = transition->GetCurrentDestinationState();
                    return true;
                }
            }
        }

        return false;
    }

    void AnimationStateMachine::UpdatePhaseCopySource()
    {
        if(auto transition = Cast<AnimationStateTransition>(mCurrentNode))
        {
            SetPhaseCopySource(transition->GetCurrentDestinationState().get());
        }
        else{
            SetPhaseCopySource(mCurrentNode.get());
        }
    }
}
