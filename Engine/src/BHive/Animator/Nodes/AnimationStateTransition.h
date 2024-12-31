#pragma once

#include "AnimationNodePoseBase.h"
#include "Animator/Jobs/JobSave.h"
#include "Animator/Jobs/JobRestore.h"

namespace BHive
{
    class AnimationState;

    class AnimationStateTransition : public AnimationNodePoseBase
    {
    public:
        AnimationStateTransition(const float duration, bool reversible);

        void Update(const AnimatorContext& context);

        bool AreConditionsSatisfied(const AnimatorContext& context) const;
        bool IsFinished(float phase);

        void SetConditonNode(const Ref<AnimationNodeBase>& node);
        void SetDestinationStateNode(const Ref<AnimationState>& node);
  
        Ref<AnimationState> GetCurrentDestinationState() const { return mDestinationStateNode; }

        void SetReversible(bool reversible);


    protected:
        void ExecuteImpl(const AnimatorContext& context, std::any& out_result);

    private:
        bool mReversible;
        Ref<AnimationNodeBase> mConditionNode;
        Ref<AnimationState> mDestinationStateNode;

        bool mReversed{false};
        Ref<AnimationState> mCurrentSource;
        Ref<AnimationState> mCurrentDestination;

        bool mSavedPoseSlotsAcquired{false};
        std::array<uint64_t, 2> mSavedPoseSlots;
        int32_t mSavedPoseSlotIndex{0};
        float mSavedPoseSourcePhase{0.f};

        JobSave mSaveSourceJob;
        JobSave mSaveTransitionJob;
        JobRestore mRestoreJob;
        JobBlend mBlendJob;
    };
} // namespace BHive
