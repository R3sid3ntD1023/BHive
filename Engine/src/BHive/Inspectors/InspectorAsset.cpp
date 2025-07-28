#include "asset/AssetManager.h"
#include "asset/EditorAssetManager.h"
#include "gui/ImGuiExtended.h"
#include "InspectorAsset.h"

#define ASSET_DRAG_DROP_NAME "ASSET"

namespace BHive
{

	bool Inspector_Asset::Inspect(FPropertyData &property_data, const bool is_read_only)
	{

		auto asset_manager = AssetManager::GetAssetManager<EditorAssetManager>();
		if (!asset_manager)
			return false;

		auto data = property_data.Value.get_value<Ref<Asset>>();
		const auto inspected_type = property_data.Value.extract_wrapped_value().get_type().get_raw_type();
		const auto type = property_data.Value.get_type();
		auto &meta_data = asset_manager->GetMetaData(Asset::GetHandle(data));
		const auto &display_name = meta_data ? meta_data.Name : "None";

		if (is_read_only)
		{
			ImGui::TextUnformatted(display_name.data());
			return false;
		}

		bool changed = false;

		const auto &meta_datas = asset_manager->GetAssetRegistry();

		if (ImGui::BeginCombo("##", display_name.c_str(), ImGuiComboFlags_PopupAlignLeft))
		{
			if (ImGui::Selectable("None", display_name == "None"))
			{
				data = nullptr;
				changed |= true;
			}

			for (auto &[id, meta] : meta_datas)
			{
				if (!(inspected_type == meta.Type || meta.Type.is_derived_from(inspected_type)))
					continue;

				auto name = meta.Name;

				ImGui::PushID(id);
				auto selected = ImGui::Selectable(name.c_str(), display_name == name);
				ImGui::PopID();

				if (selected)
				{
					data = AssetManager::GetAsset(id);

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

					data = AssetManager::GetAsset(handle);
					changed |= true;
				}
			}
			ImGui::EndDragDropTarget();
		}

		if (data)
		{
			if (ImGui::BeginItemTooltip())
			{
				ImGui::TextUnformatted(display_name.c_str());
				ImGui::EndTooltip();
			}
		}

		if (changed)
		{
			rttr::variant arg(data);
			arg.convert(type);

			property_data.Value = arg;
		}

		return changed;
	}

	REFLECT_INSPECTOR(Inspector_Asset, Asset)
} // namespace BHive
