#include "Actor.h"
#include "World.h"
#include "ActorComponent.h"
#include "scene/components/SceneComponent.h"

namespace BHive
{
    Actor::Actor()
    {
      
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

        RegisterComponent(component.get());
    }

    void Actor::RemoveComponent(ActorComponent *component_ptr)
    {
        auto it = std::find_if(mComponents.begin(), mComponents.end(), [component_ptr](const auto &component)
                               { return component.get() == component_ptr; });

        if (it == mComponents.end())
            return;

        mComponents.erase(it);
    }

    void Actor::SetLocalTransform(const FTransform &transform)
    {
        mTransform = transform;
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
        return mTransform;
    }

    FTransform Actor::GetWorldTransform() const
    {
        if (GetParent())
            return GetParent()->GetWorldTransform() * mTransform;

        return mTransform;
    }

    Actor *Actor::GetParent() const
    {
        auto parent_id = mRelationshipComponent.GetParentID();

        if (parent_id)
            return mWorld->GetActors().at(parent_id).get();

        return nullptr;
    }

    void Actor::AttachTo(Actor *actor)
    {
        if (!actor)
            return;

        if (actor->GetParent())
            actor->DetachFromParent();

        mRelationshipComponent.AddChild(actor->GetUUID());
        actor->mRelationshipComponent.SetParentID(GetUUID());
    }

    void Actor::DetachFromParent()
    {
        auto parent = GetParent();

        if (!parent)
            return;

        parent->mRelationshipComponent.RemoveChild(GetUUID());
        mRelationshipComponent.SetParentID(0);
    }

    ActorChildren Actor::GetChildren()
    {
        ActorChildren children;

        const auto& actors = mWorld->GetActors();

        for (auto &child_id : mRelationshipComponent.GetChildren())
        {
            auto actor = actors.at(child_id);
            if (actor)
                children.push_back(actor.get());
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
        writer(mRelationshipComponent);
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
        reader(mRelationshipComponent);
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

        RegisterComponents();
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
            REFLECT_CONSTRUCTOR(const Actor&)
            REFLECT_METHOD("AddComponent", &Actor::AddComponent);
    }
} // namespace BHive
