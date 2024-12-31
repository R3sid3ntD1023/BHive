#pragma once

#include "AnimationNodePoseBase.h"

namespace BHive
{

     class AnimationNodeParam : public AnimationNodePoseBase
    {
    public: 
        AnimationNodeParam(const std::string& param_name);

        void SetParameterName(const std::string& name);

        const std::string& GetParameterName() const { return mParameterName; }
    protected:
        void ExecuteImpl(const AnimatorContext& context, std::any& out_result);

    private:
        std::string mParameterName;

        REFLECTABLE()
    };

    
} // namespace BHive
