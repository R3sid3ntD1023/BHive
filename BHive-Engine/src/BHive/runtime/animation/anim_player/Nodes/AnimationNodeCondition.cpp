#include "AnimationNodeCondition.h"
#include "AnimationStateMachine.h"

namespace BHive
{
	AnimationStateCondition::AnimationStateCondition(std::optional<float> phase)
		: mPhase(phase)
	{
	}

	void AnimationStateCondition::SetPhase(std::optional<float> phase)
	{
		mPhase = phase;
	}

	void AnimationStateCondition::ExecuteImpl(const AnimPlayerContext &context, std::any &out_result)
	{
		AnimationNodeBase::ExecuteImpl(context, out_result);

		const auto state_machine = AnimationStateMachine::GetCurrent();
		ASSERT(state_machine);

		if (mPhase.has_value())
		{
			// LOG_TRACE("T Phase - {} , Phase = {}", state_machine->GetTransitionSourcePhase(), mPhase.value());
			if (state_machine->GetTransitionSourcePhase() >= mPhase.value())
			{
				out_result = true;
				return;
			}
		}

		out_result = false;
	}
} // namespace BHive