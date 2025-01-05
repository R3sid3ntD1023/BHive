#pragma once

#include "ObjectBase.h"
#include "ITickable.h"
#include "ActorComponent.h"
#include "scene/components/RelationshipComponent.h"
#include "asset/AssetType.h"
#include "ITransform.h"

namespace BHive
{
    class ActorComponent;
    class SceneComponent;
    class World;
    class Actor;

    using ComponentPtr = Ref<ActorComponent>;
    using ComponentList = std::vector<Ref<ActorComponent>>;
    using ActorChildren = std::vector<Actor *>;

    DECLARE_EVENT(OnActorDestroyed, Actor *)

    class Actor : public ObjectBase, public ITickable, public ITransform
    {
        struct FActorPostLoadData
        {
            UUID RootComponentID = 0;
            std::unordered_map<UUID, Ref<ActorComponent>> mComponentIDs;
        };

    public:
        Actor();

        virtual ~Actor() = default;

        virtual void OnBegin();

        virtual void OnUpdate(float /*deltatime*/);

        virtual void OnEnd();

        void Destroy(bool destroy_decendents = false);

        template <typename T>
        Ref<T> AddNewComponent(const std::string &name)
        {
            auto component = CreateRef<T>();
            component->SetName(name);

            AddComponent(component);

            return component;
        }

        Ref<Actor> Copy() const;

        Ref<Actor> Duplicate(bool duplicate_children = false);

        void AddComponent(const ComponentPtr &component);

        void RemoveComponent(ActorComponent *component);

        void SetLocalTransform(const FTransform &transform);

        const FTransform &GetLocalTransform() const;

        FTransform GetWorldTransform() const;

        Actor *GetParent() const;

        void AttachTo(Actor *actor);

        void DetachFromParent();

        ComponentList &GetComponents() { return mComponents; }

        const ComponentList &GetComponents() const { return mComponents; }


        ActorChildren GetChildren() const;

        World *GetWorld() { return mWorld; }

        /*ITickable*/

        virtual bool IsTickEnabled() const override { return mTickEnabled; };

        virtual void SetTickEnabled(bool) override;

        virtual void Serialize(StreamWriter &writer) const override;
        virtual void Deserialize(StreamReader &reader) override;

        bool operator==(const Actor &rhs) const;

        bool operator!=(const Actor &rhs) const;

    private:
        void RegisterComponents();
        void RegisterComponent(ActorComponent *component);

    public:
        OnActorDestroyedEvent OnActorDestroyed;

    private:
        FTransform mTransform;

        RelationshipComponent mRelationshipComponent;

        ComponentList mComponents;

        bool mTickEnabled{true};

        World *mWorld = nullptr;

        REFLECTABLEV(ObjectBase, ITickable, ITransform)

        friend class World;
    };

    REFLECT_EXTERN(Actor)

} // namespace BHive`
