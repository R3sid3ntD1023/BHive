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

    Ref<Actor> Actor::Copy() const
    {
        auto new_actor = new Actor(*this);
        new_actor->mComponents.clear();

        for (auto& component : mComponents)
        {
            auto type = component->get_type();
            rttr::variant copied = type.get_method(COPY_COMPONENT_FUNC_NAME).invoke({}, component.get());
            if (copied)
            {
                auto copied_component = copied.get_value<Ref<ActorComponent>>();
                new_actor->AddComponent(copied_component);
            }
        }
        return Ref<Actor>(new_actor);
    }


    Ref<Actor> Actor::Duplicate(bool duplicate_children) 
    {
        auto new_actor = new Actor();
        new_actor->SetLocalTransform(GetLocalTransform());
        new_actor->SetName(GetName());

        for (auto& component : mComponents)
        {
            auto type = component->get_type();
            rttr::variant duplicated = type.get_method(DUPLICATE_COMPONENT_FUNC_NAME).invoke({}, component.get());
            if (duplicated)
            {
                auto duplicated_component = duplicated.get_value<Ref<ActorComponent>>();
                new_actor->AddComponent(duplicated_component);
            }
        }
  
        if (duplicate_children)
        {
            auto children = GetChildren();
            for (auto& child : children)
            {
                auto duplicated_child = mWorld->DuplicateActor(child);
                duplicated_child->AttachTo(new_actor);
            }
        }

        return Ref<Actor>(new_actor);
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

        if (GetParent())
            DetachFromParent();

        mRelationshipComponent.SetParentID(actor->GetUUID());
        actor->mRelationshipComponent.AddChild(GetUUID());
    }

    void Actor::DetachFromParent()
    {
        auto parent = GetParent();

        if (!parent)
            return;

        parent->mRelationshipComponent.RemoveChild(GetUUID());
        mRelationshipComponent.SetParentID(0);
    }

    ActorChildren Actor::GetChildren() const
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

        writer(mTickEnabled, mTransform, mRelationshipComponent, mComponents.size());

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

        reader(mTickEnabled, mTransform, mRelationshipComponent, num_components);

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
