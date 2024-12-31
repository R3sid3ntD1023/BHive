#pragma once

#include "core/Core.h"
#include "Animator/Jobs/Jobs.h"
#include "core/UUID.h"
#include "reflection/Reflection.h"

namespace BHive
{
    class SkeletalPose;
    struct AnimatorContext;

    class AnimationNodeBase
    {
    public:
        virtual ~AnimationNodeBase() = default;

        std::any Execute(const AnimatorContext &context);

        const UUID &GetID() const { return mID; }

        virtual void CollectDesendents(std::vector<Ref<AnimationNodeBase>> &children) const {};

    protected:
        virtual void ExecuteImpl(const AnimatorContext &context, std::any &out_result);

        bool IsFirstPlay(const AnimatorContext &context) const;

        void RegisterPlay(const AnimatorContext &context) const;

    private:
        UUID mID;

        mutable std::optional<int32_t> mLastGraphPlayCounter;
        mutable bool mIsFirstPlay{true};
    };

    REFLECT(AnimationNodeBase)
    {
        BEGIN_REFLECT(AnimationNodeBase);
    }

} // namespace BHive
