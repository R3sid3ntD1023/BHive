#include "EditSubSystem.h"
#include "scene/World.h"
#include "scene/Actor.h"
#include "scene/ActorComponent.h"
#include "renderers/LineRenderer.h"

namespace BHive
{

    void Selection::Select(ObjectBase *object)
    {
        if (auto actor = Cast<Actor>(object))
        {
            mSelectedActor = actor;
        }

        mSelectedObject = object;
    }

    void Selection::Deselect(ObjectBase *object, EDeselectReason reason)
    {
        if (auto actor = Cast<Actor>(object))
        {
            if (mSelectedActor == actor)
                mSelectedActor = nullptr;

            if (reason == DeselectReason_Destroyed)
            {
                if (auto component = Cast<ActorComponent>(mSelectedObject))
                {
                    if (component->GetOwningActor() == actor)
                        mSelectedObject = nullptr;
                }
            }
        }

        if (mSelectedObject == object)
            mSelectedObject = nullptr;
    }

    void Selection::Clear()
    {
        mSelectedActor = nullptr;
        mSelectedObject = nullptr;
    }

    void EditSubSystem::OnUpdate(float dt)
    {
        if (!GetActiveWorld || !GetEditorMode)
            return;

        auto world = GetActiveWorld.invoke();
        if (!world)
            return;

        auto mode = GetEditorMode();

        if (mode != EEditorMode::EDIT)
        {
            auto physics_world = world->GetPhysicsWorld();

            if (physics_world->getIsDebugRenderingEnabled())
            {
                auto &physics_debugger = physics_world->getDebugRenderer();

                auto lines = physics_debugger.getLines();
                auto tris = physics_debugger.getTriangles();

                for (unsigned i = 0; i < lines.size(); i++)
                {
                    auto line = lines[i];
                    Line debug_line;
                    debug_line.color = line.color1;
                    debug_line.p0 = {line.point1.x, line.point1.y, line.point1.z};
                    debug_line.p1 = {line.point2.x, line.point2.y, line.point2.z};
                    LineRenderer::DrawLine(debug_line, {});
                }

                for (unsigned i = 0; i < tris.size(); i++)
                {
                    auto tri = tris[i];
                    glm::vec3 p0 = {tri.point1.x, tri.point1.y, tri.point1.z};
                    glm::vec3 p1 = {tri.point2.x, tri.point2.y, tri.point2.z};
                    glm::vec3 p2 = {tri.point3.x, tri.point3.y, tri.point3.z};
                    LineRenderer::DrawTriangle(p0, p1, p2, tri.color1);
                }
            }
        }
    }
} // namespace BHive
