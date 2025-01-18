#pragma once

#include "core/Core.h"
#include "core/UUID.h"

namespace BHive
{
	class SkeletalPose;
	struct AnimatorContext;

	class AnimationNodeBase
	{
	public:
		virtual ~AnimationNodeBase() = default;

		std::any Execute(const AnimatorContext &context);

		virtual void CollectDesendents(std::vector<Ref<AnimationNodeBase>> &children) const {};

	protected:
		virtual void ExecuteImpl(const AnimatorContext &context, std::any &out_result);

		bool IsFirstPlay(const AnimatorContext &context) const;

		void RegisterPlay(const AnimatorContext &context) const;

	private:
		mutable std::optional<int32_t> mLastGraphPlayCounter;
		mutable bool mIsFirstPlay{true};
	};

} // namespace BHive
