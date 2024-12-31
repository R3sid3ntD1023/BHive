#include "AnimationNodePoseBase.h"

namespace BHive
{
    class AnimationStateTransition;

    class AnimationState : public AnimationNodePoseBase
    {
    public:
        using Transitions = std::vector<Ref<AnimationStateTransition>>;

        AnimationState() = default;
        AnimationState(const Ref<AnimationNodePoseBase>& pose_node);

        void SetPoseNode(const Ref<AnimationNodePoseBase>& node);
        void AddTransition(const Ref<AnimationStateTransition>& transition);
        const Transitions& GetTransitions() const { return mTransitionNodes; }

        void Update(const AnimatorContext& context);

    protected:
        void ExecuteImpl(const AnimatorContext& context, std::any& out_result);

    private:
        Ref<AnimationNodePoseBase> mPoseNode;
        Transitions mTransitionNodes;
    };
} // namespace BHive
