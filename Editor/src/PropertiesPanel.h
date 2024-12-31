#pragma once

#include "core/Core.h"
#include "core/EventDelegate.h"


namespace BHive
{
    struct Component;
    class Actor;
    class ActorComponent;

    DECLARE_RET_EVENT(GetSelectedActor, Actor *)

    class PropertiesPanel
    {
    private:
        /* data */
    public:
        PropertiesPanel() = default;

        void OnGuiRender();

    private:
        void DrawActorComponents(Actor *actor);
        void DrawComponent(ActorComponent *component);
        void DrawAddComponent(Actor *actor);

        std::unordered_set<ActorComponent *> mDeletedComponents;
    };

} // namespace BHive
