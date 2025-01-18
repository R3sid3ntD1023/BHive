#pragma once

#include "AnimationNodePoseBase.h"
#include "Animator/Jobs/Jobs.h"

namespace BHive
{
	class AnimationClip;

	class AnimationClipNode : public AnimationNodePoseBase
	{
	public:
		AnimationClipNode() = default;

		void SetAnimationClip(const Ref<AnimationClip> &clip);
		void Update(const AnimatorContext &context);

	protected:
		void ExecuteImpl(const AnimatorContext &context, std::any &out_result);

	private:
		Ref<AnimationClip> mClip;
		JobClip mJobClip;
	};

} // namespace BHive
