#include "AnimationNodePoseBase.h"
#include "Animator/anim_player/AnimPlayerContext.h"

namespace BHive
{
	void AnimationNodePoseBase::Update(const AnimPlayerContext &context)
	{
		mLastUpdatePlayCounter = context.mPlayCounter;
	}

	float AnimationNodePoseBase::GetNextPhaseUnwrapped(const AnimPlayerContext &context) const
	{
		const bool copy_phase = (mPhaseRules & EPhaseRules::COPY) != 0;

		if (copy_phase)
		{
			ASSERT(mPhaseCopySource != nullptr);
			return mPhaseCopySource->GetNextPhaseUnwrapped(context);
		}

		const bool allow_sync = (mPhaseRules & EPhaseRules::SYNC) != 0;

		ASSERT(!context.mSyncPhase.has_value() || context.mSyncEnabled);
		if (context.mSyncEnabled && allow_sync)
		{
			ASSERT(context.mSyncPhase.has_value());
			return context.mSyncPhase.value();
		}

		if (mDuration == 0.0f)
			return 1.0f;

		if (IsFirstPlay(context))
			return 0.f;

		const bool reverse_direction = (mPhaseRules & EPhaseRules::REVSERSED) != 0;
		const float direction_sign = reverse_direction ? -1.f : 1.f;

		return mPhase + direction_sign * context.mDeltaTime / mDuration;
	}

	void AnimationNodePoseBase::ExecuteImpl(const AnimPlayerContext &context, std::any &out_result)
	{
		AnimationNodeBase::ExecuteImpl(context, out_result);

		if (mLastUpdatePlayCounter != context.mPlayCounter)
		{
			Update(context);
			mLastUpdatePlayCounter = context.mPlayCounter;
		}
	}

	void AnimationNodePoseBase::SetDuration(float duration)
	{
		ASSERT(duration >= 0.f);
		mDuration = duration;
	}

	void AnimationNodePoseBase::ApplyNextPhase(const AnimPlayerContext &context)
	{
		const bool copy_phase = (mPhaseRules & EPhaseRules::COPY) != 0;

		if (copy_phase)
		{
			ASSERT(mPhaseCopySource != nullptr);
			mPhase = mPhaseCopySource->GetPhase();
			return;
		}

		const float phase_unnormalized = GetNextPhaseUnwrapped(context);
		const bool allow_wrapped = (mPhaseRules & EPhaseRules::WRAP) != 0;

		if (allow_wrapped && phase_unnormalized > 1.0f)
		{
			mPhase = std::fmod(phase_unnormalized, 1.0f);
		}
		else
		{
			mPhase = std::clamp(phase_unnormalized, 0.0f, 1.0f);
		}

		ASSERT(std::isfinite(mPhase));
		ASSERT(mPhase >= 0.f && mPhase <= 1.0f);
	}

	void AnimationNodePoseBase::SetPhaseRules(uint8_t rules)
	{
		mPhaseRules = rules;
	}

	void AnimationNodePoseBase::SetPhaseRules(EPhaseRules rules, bool value)
	{
		if (value)
		{
			SetPhaseRules(mPhaseRules | rules);
		}
		else
		{
			SetPhaseRules(mPhaseRules & ~rules);
		}
	}

	void AnimationNodePoseBase::SetPhaseCopySource(const AnimationNodePoseBase *source)
	{
		mPhaseCopySource = source;
	}

} // namespace BHive
