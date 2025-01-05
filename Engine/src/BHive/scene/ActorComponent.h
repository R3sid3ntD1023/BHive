#pragma once

#include "ObjectBase.h"
#include "ITickable.h"
#include "ComponentBase.h"

namespace BHive
{
    class Actor;
    class World;

    enum ECreationMode
    {
        CreationMode_User = 0,
        CreationMode_Native
    };

    class ActorComponent : public ObjectBase, public ITickable
    {
    private:
        /* data */
    public:
        ActorComponent() = default;

        virtual ~ActorComponent() = default;

        virtual void OnBegin() {}

        virtual void OnUpdate(float /*deltatime*/) {}

        virtual void OnEnd() {};

        Actor *GetOwningActor() const { return mOwningActor; }

        World *GetWorld() const;

        template <typename T>
        T *GetOwningActor() const { return Cast<T>(GetOwningActor()); }

        virtual bool IsTickEnabled() const override { return mTickEnabled; };

        virtual void SetTickEnabled(bool) override;

        REFLECTABLEV(ObjectBase, ITickable)

    public:
        virtual void Serialize(StreamWriter &writer) const override;
        virtual void Deserialize(StreamReader &reader) override;

    private:
        bool mTickEnabled = true;

        Actor *mOwningActor = nullptr;

        friend class Actor;
    };


    template<typename TComponent>
    inline Ref<TComponent> CopyComponent(ActorComponent* component)
    {
        if (!component)
            return nullptr;

        auto component_ref = Cast<TComponent>(component);
        auto new_component = CreateRef<TComponent>(*component_ref);

        return new_component;
    }


    template<typename TComponent>
    inline Ref<TComponent> DuplicateComponent(ActorComponent* component)
    {
        if (auto new_component = CopyComponent<TComponent>(component))
        {
            new_component->GenerateNewUUID();
            return new_component;
        }
       
        return nullptr;
    }

    REFLECT(ActorComponent)
    {
        BEGIN_REFLECT(ActorComponent)
        REQUIRED_COMPONENT_FUNCS();
    }

} // namespace BHive