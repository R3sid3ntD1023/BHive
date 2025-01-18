#include "AnimationNodeParamComparison.h"
#include "Animator/AnimatorContext.h"
#include "AnimationNodeParam.h"
#include "Animator/BlackBoard.h"

namespace BHive
{
	AnimationNodeParamComparison::AnimationNodeParamComparison(
		const std::string &param_name, const std::any &value, EComparisonOperation operation)
		: mParamName(param_name),
		  mValue(value),
		  mOperation(operation)
	{
	}

	void
	AnimationNodeParamComparison::ExecuteImpl(const AnimatorContext &context, std::any &out_result)
	{
		AnimationNodeBase::ExecuteImpl(context, out_result);

		auto &blackboard = context.mBlackBoard;
		auto key = blackboard.GetKey(mParamName);
		if (!key)
		{
			out_result = false;
			return;
		}

		switch (mOperation)
		{
		case EComparisonOperation::OPERATION_EQUAL:
		{
			out_result = key->Compare(mValue);
		}
		break;

		case EComparisonOperation::OPERATION_NOTEQUAL:
		{
			out_result = !key->Compare(mValue);
		}
		break;

		default:
			break;
		}
	}
} // namespace BHive