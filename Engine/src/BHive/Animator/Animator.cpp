#include "Animator.h"
#include "mesh/SkeletalPose.h"
#include "mesh/Skeleton.h"
#include "Nodes/AnimationNodeBase.h"
#include "AnimatorContext.h"

namespace BHive
{
    Animator::Animator(const Skeleton* skeleton)
        :mQueue(skeleton)
    {
        
    }

    void Animator::Update(float dt, SkeletalPose& out_pose)
    {
        if(!mRoot) return;

        mPlayCounter++;

        AnimatorContext anim_context{.mJobQueue = mQueue, .mBlackBoard = mBlackBoard, .mDeltaTime = dt, .mPlayCounter = mPlayCounter};

        mRoot->Execute(anim_context);

        mQueue.Execute(out_pose);
        out_pose.RecalulateObjectSpaceTransforms();
    }

    void Animator::SetRootNode(const UUID &id)
    {
       mRoot = mNodes.at(id);
    }

    void Animator::AddNode(const Ref<AnimationNodeBase> &node)
    {
        mNodes.emplace(node->GetID(), node);
    }

    const Ref<AnimationNodeBase>& Animator::GetNode(const UUID &id) const
    {
        ASSERT(mNodes.contains(id));

        return mNodes.at(id); 
    }
    
    Ref<AnimationNodeBase> &Animator::GetNode(const UUID &id)
    {
         ASSERT(mNodes.contains(id));

        return mNodes.at(id); 
    }

    void Animator::SetBlackBoard(const Ref<BlackBoard> &blackboard)
    {
        mBlackBoard = blackboard;
    }
}