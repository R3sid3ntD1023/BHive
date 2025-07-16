#include "asset/AssetManager.h"
#include "asset/EditorAssetManager.h"
#include "gui/ImGuiExtended.h"
#include "InspectorAsset.h"


#define NULLPTR_NAME "Nullptr"
#define ASSET_DRAG_DROP_NAME "ASSET"

namespace BHive
{
	
	bool Inspector_AssetHandler::Inspect(
		const rttr::variant &instance, rttr::variant &var, bool read_only, const meta_getter &get_meta_data)
	{

		auto asset_manager = AssetManager::GetAssetManager<EditorAssetManager>();
		if (!asset_manager)
			return false;

		auto data = var.get_value<std::shared_ptr<Asset>>();
		auto type = var.extract_wrapped_value().get_type().get_raw_type();
		auto meta_data = asset_manager->GetMetaData(Asset::GetHandle(data));
		auto current_name = meta_data ? meta_data.Name : NULLPTR_NAME;

		if (read_only)
		{
			ImGui::TextUnformatted(current_name.data());
			return false;
		}

		bool changed = false;

		const auto &meta_datas = asset_manager->GetAssetRegistry();

		if (ImGui::BeginCombo("##", current_name.c_str(), ImGuiComboFlags_PopupAlignLeft))
		{
			if (ImGui::Selectable(NULLPTR_NAME, current_name == NULLPTR_NAME))
			{
				data = nullptr;
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
					rttr::variant arg(asset);
					arg.convert(var.get_type());
					var = arg;
					changed |= true;
					break;
				}
			}

			ImGui::EndCombo();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (auto payload = ImGui::AcceptDragDropPayload(ASSET_DRAG_DROP_NAME))
			{
				auto handle = *(UUID *)payload->Data;
				auto meta_data = asset_manager->GetMetaData(handle);
				if (meta_data.Type == type || meta_data.Type.is_derived_from(type))
				{

					auto asset = AssetManager::GetAsset(handle);
					rttr::variant arg(asset);
					arg.convert(var.get_type());
					var = arg;
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

		return changed;
	}

	REFLECT_INSPECTOR(Inspector_AssetHandler, Asset)
} // namespace BHive
