#pragma once

#include "core/Core.h"
#include "core/EventDelegate.h"
#include "EditorLayer.h"

namespace BHive
{
    class World;
    class ObjectBase;
    class Actor;

    enum EDeselectReason
    {
        DeselectReason_None,
        DeselectReason_Destroyed
    };

    struct Selection
    {

        void Select(ObjectBase *object);
        void Deselect(ObjectBase *object, EDeselectReason reason = DeselectReason_None);
        void Clear();

        ObjectBase *GetSelectedObject() const { return mSelectedObject; }
        Actor *GetSelectedActor() const { return mSelectedActor; }

    private:
        ObjectBase *mSelectedObject = nullptr;
        Actor *mSelectedActor = nullptr;
    };

    struct EditSubSystem
    {
        void OnUpdate(float dt);

        RetEventDelegate<EEditorMode> GetEditorMode;
        RetEventDelegate<Ref<World>> GetActiveWorld;

        Selection mSelection;
    };

} // namespace BHive
