#pragma once

#include "scene/ActorComponent.h"
#include "scene/ITransform.h"

namespace BHive
{
    class SceneComponent : public ActorComponent, public ITransform
    {
    public:

        void SetLocalTransform(const FTransform &transform);

        const FTransform &GetLocalTransform() const { return mTransform; }

        FTransform GetWorldTransform() const;

        virtual void Serialize(StreamWriter &writer) const;

        virtual void Deserialize(StreamReader &reader);

    private:
        FTransform mTransform;

        REFLECTABLEV(ActorComponent, ITransform)
    };

    REFLECT_EXTERN(SceneComponent)
}
