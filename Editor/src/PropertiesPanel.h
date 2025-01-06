#pragma once

#include "core/Core.h"
#include "core/EventDelegate.h"


namespace BHive
{
    struct Component;
    class Entity;
    class Component;

    DECLARE_RET_EVENT(GetSelectedEntity, Entity *)

    class PropertiesPanel
    {
    private:
        /* data */
    public:
        PropertiesPanel() = default;

        void OnGuiRender();

    private:
        void DrawComponents(Entity *entity);
        void DrawComponent(Component *component);
        void DrawAddComponent(Entity *entity);

        std::unordered_set<Component *> mDeletedComponents;
    };

} // namespace BHive
