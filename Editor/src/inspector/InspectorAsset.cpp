#include "inspector/InspectorAsset.h"
#include "asset/TAssetHandler.h"
#include "gui/ImGuiExtended.h"
#include "asset/AssetManager.h"
#include "asset/EditorAssetManager.h"
#include "asset/Factory.h"
#include "asset/FactoryRegistry.h"
#include "core/FileDialog.h"

namespace BHive
{
#define NULLPTR_NAME "Nullptr"

    bool Inspector_AssetHandler::Inspect(rttr::variant &var, bool read_only, const meta_getter &get_meta_data)
    {
        auto asset_manager = AssetManager::GetAssetManager<EditorAssetManager>();
        if (!asset_manager)
            return false;

        auto data = &var.get_value<AssetHandleBase>();

        if (!data)
            return false;

        auto type = var.extract_wrapped_value().extract_wrapped_value().get_type().get_raw_type();
        auto meta_data = asset_manager->GetMetaData(Asset::GetHandle(data->get_asset()));
        auto current_name = meta_data ? meta_data.Name : NULLPTR_NAME;

        if (read_only)
        {
            ImGui::Text("%s", current_name.data());
            return false;
        }

        bool changed = false;

        if (type)
        {

            auto meta_datas = asset_manager->GetAssetRegistry();

            if (ImGui::BeginCombo("##", current_name.c_str(), ImGuiComboFlags_PopupAlignLeft))
            {
                if (ImGui::Selectable(NULLPTR_NAME, current_name == NULLPTR_NAME))
                {
                    data->set_asset(nullptr);
                    changed |= true;
                }

                for (auto &[id, meta] : meta_datas)
                {
                    if (!(type == meta.Type || meta.Type.is_derived_from(type)))
                        continue;

                    auto name = meta.Name;

                    ImGui::PushID(id);
                    auto selected = ImGui::Selectable(name.c_str(), current_name == name);
                    ImGui::PopID();

                    if (selected)
                    {
                        auto asset = AssetManager::GetAsset(id);
                        data->set_asset(asset);
                        changed |= true;
                        break;
                    }
                }

                ImGui::EndCombo();
            }

            if (ImGui::BeginDragDropTarget())
            {
                if (auto payload = ImGui::AcceptDragDropPayload("ASSET"))
                {
                    auto handle = *(AssetHandle *)payload->Data;
                    auto meta_data = asset_manager->GetMetaData(handle);
                    if (meta_data.Type == type || type.is_derived_from(meta_data.Type))
                    {
                        data->set_asset(AssetManager::GetAsset(handle));
                        changed = true;
                    }
                }
                ImGui::EndDragDropTarget();
            }

            if (data)
            {
                if (ImGui::BeginItemTooltip())
                {
                    ImGui::TextUnformatted(current_name.c_str());
                    ImGui::EndTooltip();
                }
            }
        }

        return changed;
    }

    REFLECT_INSPECTOR(Inspector_AssetHandler, AssetHandleBase)

} // namespace BHive
