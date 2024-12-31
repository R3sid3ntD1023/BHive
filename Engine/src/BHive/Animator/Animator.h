#pragma once

#include "core/Core.h"
#include "Jobs/JobQueue.h"
#include "core/UUID.h"
#include "reflection/Reflection.h"

namespace BHive
{
    class SkeletalPose;
    class Skeleton;
    class AnimationNodeBase;
    class BlackBoard;
   
    class Animator
    {
    public:
        Animator() = default;
        Animator(const Skeleton* skeleton);

        void Update(float dt, SkeletalPose& out_pose);

        void SetRootNode(const UUID& id);

        void AddNode(const Ref<AnimationNodeBase>& node);

        template<typename TNode, typename = std::enable_if_t<std::is_base_of_v<AnimationNodeBase, TNode>>,
        typename... TArgs>
        Ref<TNode> Create(TArgs&&... args)
        {
            auto node = CreateRef<TNode>(std::forward<TArgs>(args)...);
            mNodes.emplace(node->GetID(), node);
            return Cast<TNode>(node);
        }

        template<typename... TArgs>
        Ref<AnimationNodeBase> Create(const rttr::type& type,  TArgs&&... args)
        {
            auto node = type.create(std::forward<TArgs>(args)...).get_value<Ref<AnimationNodeBase>>();
            mNodes.emplace(node->GetID(), node);
            return node;
        }

        const Ref<AnimationNodeBase>& GetNode(const UUID& id) const ;
        Ref<AnimationNodeBase>& GetNode(const UUID& id);

        const std::unordered_map<UUID, Ref<AnimationNodeBase>>& GetNodes() const { return mNodes;}

        void SetBlackBoard(const Ref<BlackBoard>& blackboard);

        Ref<BlackBoard> GetBlackBoard() { return mBlackBoard; }

    private:
        JobQueue mQueue;
        std::unordered_map<UUID, Ref<AnimationNodeBase>> mNodes;
        Ref<BlackBoard> mBlackBoard;
        Ref<AnimationNodeBase> mRoot;
        int32_t mPlayCounter{0};
    };
  
} // namespace BHive
