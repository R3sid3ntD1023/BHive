#include "Entity.h"
#include "World.h"
#include "Component.h"
#include "scene/components/SceneComponent.h"

namespace BHive
{
    Entity::Entity()
    {
      
    }

    void Entity::OnBegin()
    {
        for (auto &component : mComponents)
            component->OnBegin();
    }

    void Entity::OnUpdate(float dt)
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

    void Entity::OnEnd()
    {
        for (auto &component : mComponents)
            component->OnEnd();
    }

    void Entity::Destroy(bool destroy_decendents)
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

        OnEntityDestroyed.invoke(this);
    }

    void Entity::AddComponent(const ComponentPtr &component)
    {
        mComponents.push_back(component);

        RegisterComponent(component.get());
    }

    void Entity::RemoveComponent(Component *component_ptr)
    {
        auto it = std::find_if(mComponents.begin(), mComponents.end(), [component_ptr](const auto &component)
                               { return component.get() == component_ptr; });

        if (it == mComponents.end())
            return;

        mComponents.erase(it);
    }

    Ref<Entity> Entity::Copy() const
    {
        auto new_entity = new Entity(*this);
        new_entity->mComponents.clear();

        for (auto& component : mComponents)
        {
            auto type = component->get_type();
            rttr::variant copied = type.get_method(COPY_COMPONENT_FUNC_NAME).invoke({}, component.get());
            if (copied)
            {
                auto copied_component = copied.get_value<Ref<Component>>();
                new_entity->AddComponent(copied_component);
            }
        }
        return Ref<Entity>(new_entity);
    }


    Ref<Entity> Entity::Duplicate(bool duplicate_children) 
    {
        auto new_entity = new Entity();
        new_entity->SetLocalTransform(GetLocalTransform());
        new_entity->SetName(GetName());

        for (auto& component : mComponents)
        {
            auto type = component->get_type();
            rttr::variant duplicated = type.get_method(DUPLICATE_COMPONENT_FUNC_NAME).invoke({}, component.get());
            if (duplicated)
            {
                auto duplicated_component = duplicated.get_value<Ref<Component>>();
                new_entity->AddComponent(duplicated_component);
            }
        }
  
        if (duplicate_children)
        {
            auto children = GetChildren();
            for (auto& child : children)
            {
                auto duplicated_child = mWorld->DuplicateEntity(child);
                duplicated_child->AttachTo(new_entity);
            }
        }

        return Ref<Entity>(new_entity);
    }

  
    void Entity::RegisterComponents()
    {
        for (auto &component : mComponents)
        {
            RegisterComponent(component.get());
        }
    }

    void Entity::RegisterComponent(Component *component)
    {
        if (!component)
            return;

        component->mOwningentity = this;
    }

    void Entity::SetLocalTransform(const FTransform& transform)
    {
        mTransform = transform;

        FTransform parent;
        if (GetParent())
            parent = GetParent()->GetWorldTransform();

        mWorldTransform = parent * mTransform;

        for (auto child : GetChildren())
            child->UpdateWorldTransform();

        for (auto& component : mComponents)
        {
            if (auto scene_component = Cast<SceneComponent>(component))
                scene_component->UpdateWorldTransform();
        }

      
    }


    const FTransform &Entity::GetLocalTransform() const
    {
        return mTransform;
    }

    void Entity::SetWorldTransform(const FTransform& transform)
    {
        mWorldTransform = transform;

        FTransform parent;
        if(GetParent())
            parent = GetParent()->GetWorldTransform();

        mTransform = parent.inverse() * mWorldTransform;


        for (auto child : GetChildren())
            child->UpdateWorldTransform();

        for (auto& component : mComponents)
        {
            if (auto scene_component = Cast<SceneComponent>(component))
                scene_component->UpdateWorldTransform();
        }

    }

    const FTransform& Entity::GetWorldTransform() const
    {
        return mWorldTransform;
    }

    void Entity::UpdateWorldTransform()
    {    
        FTransform parent;
        if (GetParent())
            parent = GetParent()->GetWorldTransform();

        mWorldTransform = parent * mTransform;

        for (auto& component : mComponents)
        {
            if (auto scene_component = Cast<SceneComponent>(component))
                scene_component->UpdateWorldTransform();
        }
    }

    Entity *Entity::GetParent() const
    {
        auto parent_id = mRelationshipComponent.GetParentID();

        if (parent_id)
            return mWorld->GetEntities().at(parent_id).get();

        return nullptr;
    }

    void Entity::AttachTo(Entity *entity)
    {
        if (!entity)
            return;

        if (GetParent())
            DetachFromParent();

        mRelationshipComponent.SetParentID(entity->GetUUID());
        entity->mRelationshipComponent.AddChild(GetUUID());
    }

    void Entity::DetachFromParent()
    {
        auto parent = GetParent();

        if (!parent)
            return;

        parent->mRelationshipComponent.RemoveChild(GetUUID());
        mRelationshipComponent.SetParentID(0);
    }

    EntityChildren Entity::GetChildren() const
    {
        EntityChildren children;

        const auto& entities = mWorld->GetEntities();

        for (auto &child_id : mRelationshipComponent.GetChildren())
        {
            auto entity = entities.at(child_id);
            if (entity)
                children.push_back(entity.get());
        }

        return children;
    }

    void Entity::SetTickEnabled(bool enabled)
    {
        mTickEnabled = enabled;
    }

    void Entity::Serialize(StreamWriter &ar) const
    {
        ObjectBase::Serialize(ar);

        ar(mTickEnabled, mTransform, mWorldTransform, mRelationshipComponent, mComponents.size());

        for (auto &component : mComponents)
        {
            ar(component->get_type());
            component->Serialize(ar);
        }
    }

    void Entity::Deserialize(StreamReader &ar)
    {
        ObjectBase::Deserialize(ar);

        size_t num_components = 0;

        ar(mTickEnabled, mTransform, mWorldTransform, mRelationshipComponent, num_components);

        if (mComponents.size() < num_components)
            mComponents.resize(num_components);

        for (size_t i = 0; i < num_components; i++)
        {
            AssetType component_type = InvalidType;
            ar(component_type);

            auto &component = mComponents[i];
            if (!component)
            {
                component = component_type.create().get_value<Ref<Component>>();
            }

            component->Deserialize(ar);
        }

        RegisterComponents();
    }

   

    bool Entity::operator==(const Entity &rhs) const
    {
        return GetUUID() == rhs.GetUUID();
    }

    bool Entity::operator!=(const Entity &rhs) const
    {
        return !(*this == rhs);
    }

    REFLECT(Entity)
    {
        BEGIN_REFLECT(Entity)
        REFLECT_CONSTRUCTOR()
        REFLECT_CONSTRUCTOR(const Entity&)
        REFLECT_METHOD("AddComponent", &Entity::AddComponent);
    }
} // namespace BHive
