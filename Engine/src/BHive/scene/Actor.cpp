#include "Actor.h"
#include "World.h"
#include "ActorComponent.h"
#include "scene/components/SceneComponent.h"

namespace BHive
{
    Actor::Actor()
    {
        mRootComponent = AddNewComponent<SceneComponent>("Root");
    }

    void Actor::OnBegin()
    {
        for (auto &component : mComponents)
            component->OnBegin();
    }

    void Actor::OnUpdate(float dt)
    {
        if (!IsTickEnabled())
            return;

        for (auto &component : mComponents)
        {
            if (!component->IsTickEnabled())
                continue;

            component->OnUpdate(dt);
        }
    }

    void Actor::OnEnd()
    {
        for (auto &component : mComponents)
            component->OnEnd();
    }

    void Actor::Destroy(bool destroy_decendents)
    {
        auto children = GetChildren();
        for (auto child : children)
        {
            child->DetachFromParent();
            if (destroy_decendents)
            {
                child->Destroy(destroy_decendents);
            }
        }

        DetachFromParent();

        OnActorDestroyed.invoke(this);
    }

    void Actor::AddComponent(const ComponentPtr &component)
    {
        mComponents.push_back(component);

        if (auto scenecomponent = Cast<SceneComponent>(component))
        {
            if (mRootComponent && *mRootComponent != *scenecomponent)
            {

                scenecomponent->AttachTo(mRootComponent.get());
            }
            else if (!mRootComponent)
            {
                SetRootComponent(scenecomponent);
            }
        }

        RegisterComponent(component.get());
    }

    void Actor::RemoveComponent(ActorComponent *component_ptr)
    {
        auto it = std::find_if(mComponents.begin(), mComponents.end(), [component_ptr](const auto &component)
                               { return component.get() == component_ptr; });

        if (it == mComponents.end())
            return;

        if (auto scenecomponent = Cast<SceneComponent>(*it))
        {
            for (auto child : scenecomponent->GetChildren())
            {
                child->DetachFromParent();
            }

            scenecomponent->DetachFromParent();
        }

        mComponents.erase(it);
    }

    void Actor::SetRootComponent(Ref<SceneComponent> &component)
    {
        mRootComponent = component;
    }

    void Actor::SetLocalTransform(const FTransform &transform)
    {
        if (mRootComponent)
            mRootComponent->SetLocalTransform(transform);
    }

    void Actor::RegisterComponents()
    {
        for (auto &component : mComponents)
        {
            RegisterComponent(component.get());
        }
    }

    void Actor::RegisterComponent(ActorComponent *component)
    {
        if (!component)
            return;

        component->mOwningActor = this;
    }

    const FTransform &Actor::GetLocalTransform() const
    {
        static FTransform sTransform;

        if (mRootComponent)
            return mRootComponent->GetLocalTransform();

        return sTransform;
    }

    FTransform Actor::GetWorldTransform() const
    {
        static FTransform sTransform;

        if (mRootComponent)
            return mRootComponent->GetWorldTransform();

        return sTransform;
    }

    Actor *Actor::GetParent() const
    {
        if (mRootComponent && mRootComponent->GetParent())
            return mRootComponent->GetParent()->GetOwningActor();

        return nullptr;
    }

    void Actor::AttachTo(Actor *actor)
    {
        if (!mRootComponent || !(actor && actor->mRootComponent))
            return;

        mRootComponent->AttachTo(actor->mRootComponent.get());
    }

    void Actor::DetachFromParent()
    {
        mRootComponent->DetachFromParent();
    }

    ActorChildren Actor::GetChildren()
    {
        ActorChildren children;

        if (!mRootComponent)
            return children;

        for (auto &child : mRootComponent->GetChildren())
        {
            if (auto actor = child->GetOwningActor())
            {
                if (*actor != *this)
                {
                    children.push_back(actor);
                }
            }
        }

        return children;
    }

    void Actor::SetTickEnabled(bool enabled)
    {
        mTickEnabled = enabled;
    }

    void Actor::Serialize(StreamWriter &writer) const
    {
        ObjectBase::Serialize(writer);

        writer(mTickEnabled);
        writer(mPostLoadData.RootComponentID);
        writer(mComponents.size());

        for (auto &component : mComponents)
        {
            writer(component->get_type());
            component->Serialize(writer);
        }
    }

    void Actor::Deserialize(StreamReader &reader)
    {
        ObjectBase::Deserialize(reader);

        size_t num_components = 0;

        reader(mTickEnabled);
        reader(mPostLoadData.RootComponentID);
        reader(num_components);

        if (mComponents.size() < num_components)
            mComponents.resize(num_components);

        for (size_t i = 0; i < num_components; i++)
        {
            AssetType component_type = InvalidType;
            reader(component_type);

            auto &component = mComponents[i];
            if (!component)
            {
                component = component_type.create().get_value<Ref<ActorComponent>>();
            }

            component->Deserialize(reader);
        }

        auto root_id = mPostLoadData.RootComponentID;
        auto it = std::find_if(mComponents.begin(), mComponents.end(), [root_id](const auto &component)
                               { return component->GetUUID() == root_id; });

        if (it != mComponents.end())
            mRootComponent = Cast<SceneComponent>(*it);

        RegisterComponents();
    }

    void Actor::OnPreSave()
    {
        mPostLoadData = FActorPostLoadData{};
        mPostLoadData.RootComponentID = mRootComponent->GetUUID();

        for (auto &component : mComponents)
        {
            mPostLoadData.mComponentIDs.emplace(component->GetUUID(), component);
        }

        for (auto &component : mComponents)
            component->OnPreSave();
    }

    void Actor::OnPostLoad()
    {
        for (auto &component : mComponents)
            component->OnPostLoad();
    }

    bool Actor::operator==(const Actor &rhs) const
    {
        return GetUUID() == rhs.GetUUID();
    }

    bool Actor::operator!=(const Actor &rhs) const
    {
        return !(*this == rhs);
    }

    REFLECT(Actor)
    {
        BEGIN_REFLECT(Actor)
        REFLECT_CONSTRUCTOR()
        REFLECT_CONSTRUCTOR(const Actor &)
        REFLECT_METHOD("AddComponent", &Actor::AddComponent);
    }
} // namespace BHive
