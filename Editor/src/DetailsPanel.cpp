#include "DetailsPanel.h"
#include "inspector/Inspectors.h"
#include "gui/ImGuiExtended.h"
#include "subsystem/SubSystem.h"
#include "subsystems/EditSubSystem.h"

namespace BHive
{
    void DetailsPanel::OnGuiRender()
    {
        if (ImGui::Begin("Details Panel"))
        {
            auto &edit_subsystem = SubSystemContext::Get().GetSubSystem<EditSubSystem>();

            auto object = edit_subsystem.mSelection.GetSelectedObject();

            if (object)
            {
                rttr::variant obj_var = object;
                if (inspect(obj_var))
                {
                    object = obj_var.get_value<ObjectBase *>();
                }
            }
        }

        ImGui::End();
    }
}