#pragma once

#include "core/Core.h"

namespace BHive
{
	class SkeletalPose;
	struct AnimPlayerContext;

	class AnimationNodeBase
	{
	public:
		virtual ~AnimationNodeBase() = default;

		std::any Execute(const AnimPlayerContext &context);

		virtual void CollectDesendents(std::vector<Ref<AnimationNodeBase>> &children) const {};

	protected:
		virtual void ExecuteImpl(const AnimPlayerContext &context, std::any &out_result);

		bool IsFirstPlay(const AnimPlayerContext &context) const;

		void RegisterPlay(const AnimPlayerContext &context) const;

	private:
		mutable std::optional<int32_t> mLastGraphPlayCounter;
		mutable bool mIsFirstPlay{true};
	};

} // namespace BHive
