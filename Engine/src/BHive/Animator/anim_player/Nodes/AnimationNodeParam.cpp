#include "AnimationNodeParam.h"
#include "Animator/anim_player/AnimPlayerContext.h"
#include "Animator/anim_player/BlackBoard.h"

namespace BHive
{
	AnimationNodeParam::AnimationNodeParam(const std::string &param_name)
		: mParameterName(param_name)
	{
	}

	void AnimationNodeParam::SetParameterName(const std::string &name)
	{
		mParameterName = name;
	}

	void AnimationNodeParam::ExecuteImpl(const AnimPlayerContext &context, std::any &out_result)
	{
		AnimationNodePoseBase::ExecuteImpl(context, out_result);

		auto &blackboard = context.mBlackBoard;

		auto key = blackboard.GetKey(mParameterName);
		if (!key)
		{
			out_result = false;
			return;
		}

		out_result = key->Get();
	}

} // namespace BHive
