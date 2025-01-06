#pragma once

#include "ObjectBase.h"
#include "ITickable.h"
#include "ComponentBase.h"

namespace BHive
{
    class Entity;
    class World;

    enum ECreationMode
    {
        CreationMode_User = 0,
        CreationMode_Native
    };

    class Component : public ObjectBase, public ITickable
    {
    private:
        /* data */
    public:
        Component() = default;

        virtual ~Component() = default;

        virtual void OnBegin() {}

        virtual void OnUpdate(float /*deltatime*/) {}

        virtual void OnEnd() {};

        Entity *GetOwner() const { return mOwningentity; }

        World *GetWorld() const;

        template <typename T>
        T *GetOwner() const { return Cast<T>(GetOwner()); }

        virtual bool IsTickEnabled() const override { return mTickEnabled; };

        virtual void SetTickEnabled(bool) override;

        REFLECTABLEV(ObjectBase, ITickable)

    public:
        virtual void Serialize(StreamWriter &ar) const override;
        virtual void Deserialize(StreamReader &ar) override;

    private:
        bool mTickEnabled = true;

        Entity *mOwningentity = nullptr;

        friend class Entity;
    };


    template<typename TComponent>
    inline Ref<TComponent> CopyComponent(Component* component)
    {
        if (!component)
            return nullptr;

        auto component_ref = Cast<TComponent>(component);
        auto new_component = CreateRef<TComponent>(*component_ref);

        return new_component;
    }


    template<typename TComponent>
    inline Ref<TComponent> DuplicateComponent(Component* component)
    {
        if (auto new_component = CopyComponent<TComponent>(component))
        {
            new_component->GenerateNewUUID();
            return new_component;
        }
       
        return nullptr;
    }

    REFLECT(Component)
    {
        BEGIN_REFLECT(Component)
        REQUIRED_COMPONENT_FUNCS();
    }

} // namespace BHive