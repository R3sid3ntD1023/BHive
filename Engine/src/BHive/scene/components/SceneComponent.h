#pragma once

#include "scene/Component.h"
#include "scene/ITransform.h"
#include "RelationshipComponent.h"

namespace BHive
{
    class SceneComponent : public Component, public ITransform
    {
    public:
        void SetWorldTransform(const FTransform& transform);

        const FTransform& GetWorldTransform() const;

        void SetLocalTransform(const FTransform& transform);

        const FTransform& GetLocalTransform() const;


        virtual void Serialize(StreamWriter &ar) const;

        virtual void Deserialize(StreamReader &ar);

    private:
        void UpdateWorldTransform();

    private:
        FTransform mLocalTransform;
        FTransform mWorldTransform;

        REFLECTABLEV(Component, ITransform)

         friend class Entity;
    };

    REFLECT_EXTERN(SceneComponent)
}
