#include "asset/AssetManager.h"
#include "asset/EditorAssetManager.h"
#include "gui/ImGuiExtended.h"
#include "InspectorAsset.h"
#include "project/Project.h"
#include "gui/PayloadHelpers.h"

#define ASSET_DRAG_DROP_NAME "CONTENT_BROWSER_ITEM"

namespace BHive
{

	template <typename T>
	bool Inspector_Asset<T>::inspect(const rttr::variant &owner, rttr::variant &var, const MetaGetter &GetMetaData, const bool is_read_only)
	{

		auto asset_manager = AssetManager::GetAssetManager<EditorAssetManager>();
		if (!asset_manager)
			return false;

		auto data = rttr::variant_cast<Ref<T>>(var);

		const auto inspected_type = var.extract_wrapped_value().get_type().get_raw_type();
		const auto type = var.get_type();
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
					data = AssetManager::GetAsset<T>(id);
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
				auto buffer = (const char *)payload->Data;
				auto entries = PayloadHelpers::GetEntriesFromBuffer(buffer, payload->DataSize);
				if (!entries.empty())
				{
					auto relative = Project::GetResourceRelativePath(entries[0]);
					auto handle = asset_manager->GetHandle(relative);
					auto handle_meta_data = asset_manager->GetMetaData(handle);
					if (handle_meta_data.Type == inspected_type || handle_meta_data.Type.is_derived_from(inspected_type))
					{
						data = AssetManager::GetAsset<T>(handle);
						changed |= true;
					}
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
			var = data;
		}

		return changed;
	}
} // namespace BHive
