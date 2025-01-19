#pragma once

#include "AnimationNodeBase.h"

namespace BHive
{
    class AnimationStateCondition : public AnimationNodeBase
    {
    public:
        AnimationStateCondition() = default;
        AnimationStateCondition(std::optional<float> phase);

        std::optional<float> GetPhase() const { return mPhase; };

        void SetPhase(std::optional<float> phase);

    protected:  
        void ExecuteImpl(const AnimPlayerContext& context, std::any& out_result) override;

    private:
        std::optional<float> mPhase;
    };
} // namespace BHive
