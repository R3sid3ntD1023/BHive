#pragma once

#include "AnimationNodeBase.h"

namespace BHive
{
    class AnimationNodePoseBase : public AnimationNodeBase
    {
    public:     

        virtual void Update(const AnimatorContext& context);

        float GetDuration() const { return mDuration; };

        float GetPhase() const { return mPhase; };

        float GetNextPhaseUnwrapped(const AnimatorContext& context) const;
        
        void SetDuration(float duration);

    protected:
          enum EPhaseRules : uint8_t { COPY = BIT(0), WRAP = BIT(1), SYNC = BIT(2), REVSERSED = BIT(3)};

        virtual void ExecuteImpl(const AnimatorContext& context, std::any& out_result);

      
        void ApplyNextPhase(const AnimatorContext& context);

        void SetPhaseRules(uint8_t rules);

        void SetPhaseRules(EPhaseRules rules, bool value);

        void SetPhaseCopySource(const AnimationNodePoseBase* source);

private:
        float mDuration{0.f};

        float mPhase{0.f};

        uint8_t mPhaseRules{EPhaseRules::WRAP | EPhaseRules::SYNC};
        const AnimationNodePoseBase* mPhaseCopySource{nullptr};

        std::optional<int32_t> mLastUpdatePlayCounter;

    };
} // namespace BHive
