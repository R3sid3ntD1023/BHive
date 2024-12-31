#include "PropertiesPanel.h"
#include "reflection/Reflection.h"
#include "gui/ImGuiExtended.h"
#include "inspector/Inspectors.h"
#include "scene/Actor.h"
#include "subsystem/SubSystem.h"
#include "subsystems/EditSubSystem.h"
#include "scene/components/SceneComponent.h"

namespace BHive
{
    void PropertiesPanel::OnGuiRender()
    {

        auto &edit_subsystem = SubSystemContext::Get().GetSubSystem<EditSubSystem>();
        if (auto actor = edit_subsystem.mSelection.GetSelectedActor())
        {
            DrawActorComponents(actor);

            ImGui::SeparatorText("Details");

            DrawAddComponent(actor);

            if (mDeletedComponents.size())
            {
                for (auto component : mDeletedComponents)
                {
                    if (edit_subsystem.mSelection.GetSelectedObject() == component)
                        edit_subsystem.mSelection.Deselect(component);

                    actor->RemoveComponent(component);
                }

                mDeletedComponents.clear();
            }
        }
    }

    void PropertiesPanel::DrawActorComponents(Actor *actor)
    {
        for (auto &component : actor->GetComponents())
        {
            if (auto scene_component = Cast<SceneComponent>(component))
            {
                auto parent = scene_component->GetParent();
                if (parent && parent->GetOwningActor() == actor)
                    continue;
            }

            DrawComponent(component.get());
        }
    }

    void PropertiesPanel::DrawComponent(ActorComponent *component)
    {
        bool destroyed = false;

        ImGui::PushID((uint64_t)component->GetUUID());

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;

        auto scenecomponent = Cast<SceneComponent>(component);
        bool is_scene_component = scenecomponent != nullptr;

        flags |= (is_scene_component && scenecomponent->GetChildren().size()) ? 0 : ImGuiTreeNodeFlags_Leaf;

        bool opened = ImGui::TreeNodeEx(component->GetName().c_str(), flags);

        if (ImGui::IsItemClicked() || ImGui::IsItemFocused())
        {
            auto &edit_subsystem = SubSystemContext::Get().GetSubSystem<EditSubSystem>();
            edit_subsystem.mSelection.Select(component);
        }

        if (ImGui::BeginPopupContextItem("ComponentContextMenu"))
        {
            if (ImGui::MenuItem("Remove"))
            {
                destroyed = true;
            }
            ImGui::EndPopup();
        }

        if (opened)
        {
            if (is_scene_component)
            {
                for (auto &child : scenecomponent->GetChildren())
                {
                    auto parent = child->GetOwningActor();
                    if (parent && parent == component->GetOwningActor())
                    {
                        DrawComponent(child);
                    }
                }
            }
            ImGui::TreePop();
        }

        ImGui::PopID();

        if (destroyed)
            mDeletedComponents.insert(component);
    }

    void PropertiesPanel::DrawAddComponent(Actor *actor)
    {
        static auto add_component_id = "AddComponent";
        static auto derived_component_types = rttr::type::get<ActorComponent>().get_derived_classes();

        auto line_height = ImGui::GetLineHeight();
        auto button_size = ImVec2(200, line_height);
        auto pos = ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x * .5f) - (button_size.x * .5f);
        ImGui::SetCursorPosX(pos);
        ImGui::PushStyleColor(ImGuiCol_Button, {0, .5f, 0, 1});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, {0, .5f, 0, 1});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {.2f, .6f, .2f, 1});

        if (ImGui::Button("Add Component", button_size))
        {
            ImGui::OpenPopup(add_component_id);
        }

        ImGui::PopStyleColor(3);

        if (ImGui::BeginPopup(add_component_id))
        {
            for (auto &type : derived_component_types)
            {
                auto spawnable_var = type.get_metadata(ClassMetaData_ComponentSpawnable);
                auto is_spawnable = spawnable_var ? spawnable_var.to_bool() : false;

                if (!is_spawnable)
                    continue;

                if (ImGui::Selectable(type.get_name().data()))
                {
                    auto component = type.create().get_value<Ref<ActorComponent>>();
                    component->SetName(std::string("New") + type.get_name());
                    actor->AddComponent(component);
                }
            }
            ImGui::EndPopup();
        }
    }

} // namespace BHive
