#pragma once

#include "AnimationNodeBase.h"

namespace BHive
{
    enum class EComparisonOperation
    {
        OPERATION_EQUAL,
        OPERATION_NOTEQUAL
    };

    class AnimationNodeParamComparison : public AnimationNodeBase
    {
    public:
        AnimationNodeParamComparison(const std::string& param_name, const std::any& value, EComparisonOperation operation);

    protected:
        void ExecuteImpl(const AnimatorContext& context, std::any& out_result) override;

    private:
        std::string mParamName;
        std::any mValue;
        EComparisonOperation mOperation;
    };
} // namespace BHive
