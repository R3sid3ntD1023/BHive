#include "SceneComponent.h"
#include "scene/Actor.h"
#include "scene/World.h"

namespace BHive
{
    void SceneComponent::SetLocalTransform(const FTransform &transform)
    {
        mTransform = transform;
    }

    void SceneComponent::AttachTo(SceneComponent *parentComponent)
    {
        DetachFromParent();

        if (!parentComponent)
            return;

        mParentComponent = parentComponent;
        mParentComponent->mChildren.push_back(this);
    }

    void SceneComponent::DetachFromParent()
    {
        if (!mParentComponent)
            return;

        auto it = std::find(mParentComponent->mChildren.begin(), mParentComponent->mChildren.end(), this);
        if (it == mParentComponent->mChildren.end())
            return;

        mParentComponent->mChildren.erase(it);
        mParentComponent = nullptr;
    }

    FTransform SceneComponent::GetWorldTransform() const
    {
        if (mParentComponent)
        {
            return mParentComponent->GetWorldTransform() * mTransform;
        }

        return mTransform;
    }

    void SceneComponent::Serialize(StreamWriter &writer) const
    {
        ActorComponent::Serialize(writer);

        writer(mTransform, mPostLoadData.mParentID, mPostLoadData.mChildrenIDs);
    }

    void SceneComponent::Deserialize(StreamReader &reader)
    {
        ActorComponent::Deserialize(reader);

        reader(mTransform, mPostLoadData.mParentID, mPostLoadData.mChildrenIDs);
    }

    void SceneComponent::OnPreSave()
    {
        mPostLoadData = FSceneComponentPostLoadData{};

        if (mParentComponent)
            mPostLoadData.mParentID = {mParentComponent->GetOwningActor()->GetUUID(), mParentComponent->GetUUID()};

        for (auto &child : mChildren)
            mPostLoadData.mChildrenIDs.push_back({child->GetOwningActor()->GetUUID(), child->GetUUID()});
    }

    void SceneComponent::OnPostLoad()
    {
        auto world = GetWorld();

        if (mPostLoadData.mParentID.first)
        {
            auto actor = world->mActors.at(mPostLoadData.mParentID.first);
            auto &actor_components = actor->GetComponents();
            auto id = mPostLoadData.mParentID.second;

            auto it = std::find_if(actor_components.begin(), actor_components.end(), [id](const auto &component)
                                   { return component->GetUUID() == id; });

            if (it != actor_components.end())
                mParentComponent = Cast<SceneComponent>(it->get());
        }

        for (auto &child_id : mPostLoadData.mChildrenIDs)
        {
            auto actor = world->mActors.at(child_id.first);
            auto &actor_components = actor->GetComponents();
            auto id = child_id.second;

            auto it = std::find_if(actor_components.begin(), actor_components.end(), [id](const auto &component)
                                   { return component->GetUUID() == id; });

            if (it != actor_components.end())
            {
                auto scene_component = Cast<SceneComponent>(it->get());
                scene_component->AttachTo(this);
            }
        }
    }

    REFLECT(SceneComponent)
    {
        BEGIN_REFLECT(SceneComponent)
        (META_DATA(ClassMetaData_ComponentSpawnable, true))
            REQUIRED_COMPONENT_FUNCS();
    }

}