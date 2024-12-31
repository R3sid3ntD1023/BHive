#pragma once

#include "scene/ActorComponent.h"
#include "scene/ITransform.h"

namespace BHive
{
    using SceneComponentPtr = Ref<class SceneComponent>;
    using ComponentChildren = std::vector<class SceneComponent *>;

    struct FSceneComponentPostLoadData
    {
        std::pair<UUID, UUID> mParentID = {0, 0};
        std::vector<std::pair<UUID, UUID>> mChildrenIDs;
    };

    class SceneComponent : public ActorComponent, public ITransform
    {
    public:
        void AttachTo(SceneComponent *parentComponent);

        void DetachFromParent();

        void SetLocalTransform(const FTransform &transform);

        const FTransform &GetLocalTransform() const { return mTransform; }

        FTransform GetWorldTransform() const;

        const ComponentChildren &GetChildren() const { return mChildren; }

        ComponentChildren &GetChildren() { return mChildren; }

        SceneComponent *GetParent() const { return mParentComponent; }

        virtual void Serialize(StreamWriter &writer) const;

        virtual void Deserialize(StreamReader &reader);

        virtual void OnPreSave();
        virtual void OnPostLoad();

    private:
        FTransform mTransform;

        SceneComponent *mParentComponent = nullptr;
        ComponentChildren mChildren;
        FSceneComponentPostLoadData mPostLoadData;

        REFLECTABLEV(ActorComponent, ITransform)
    };

    REFLECT_EXTERN(SceneComponent)
}
