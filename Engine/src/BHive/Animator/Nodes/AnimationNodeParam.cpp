#include "AnimationNodeParam.h"
#include "Animator/AnimatorContext.h"
#include "Animator/BlackBoard.h"

namespace BHive
{
     AnimationNodeParam::AnimationNodeParam(const std::string &param_name)
        :mParameterName(param_name)
    {
    }

    void AnimationNodeParam::SetParameterName(const std::string &name)
    {
        mParameterName = name;
    }

    void AnimationNodeParam::ExecuteImpl(const AnimatorContext& context, std::any& out_result)
    {
        AnimationNodePoseBase::ExecuteImpl(context, out_result);

        auto blackboard = context.mBlackBoard;
        if(!blackboard)
        {
            out_result = false;
            return;
        }

        auto key = blackboard->GetKey(mParameterName);
        if(!key)
        {
            out_result = false;
            return;
        }

        out_result = key->Get();
    }

} // namespace BHive

