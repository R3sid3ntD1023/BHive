#pragma once

#include "AnimationNodePoseBase.h"
#include <stack>

namespace BHive
{
    class AnimationState;
    class AnimationStateTransition;

    class AnimationStateMachine : public AnimationNodePoseBase
    {
    public:
        AnimationStateMachine();

        static const AnimationStateMachine* GetCurrent();

        void Update(const AnimatorContext& context);

        void AddState(const Ref<AnimationState>& node);

        Ref<AnimationState> GetTransitionSource() const { return mTransitionSource; }

        float GetTransitionSourcePhase() const { return mTransitionSourceCandidatePhase; };

    protected:
        void ExecuteImpl(const AnimatorContext& context, std::any& out_result) override;

        bool UpdateState(const AnimatorContext& context, float pending_phase);

        void UpdatePhaseCopySource();

        inline static std::stack<AnimationStateMachine*> sActiveStateMachines;

        std::vector<Ref<AnimationState>> mNodes;

        Ref<AnimationNodePoseBase> mCurrentNode;
        Ref<AnimationState> mTransitionSourceCandidate;
        float mTransitionSourceCandidatePhase{0.f};
        Ref<AnimationState> mTransitionSource{nullptr};
    };
} // namespace BHive
