#include "InspectorPath.h"
#include "gui/ImGuiExtended.h"
#include "core/platform/Platform.h"

namespace BHive
{
	bool Inspector_Path::inspect(const rttr::variant &owner, rttr::variant &var, const MetaGetter &GetMetaData, const bool is_read_only)
	{
		auto data = var.get_value<std::filesystem::path>();
		auto data_str = data.string();
		bool changed = false;

		if (is_read_only)
		{
			ImGui::TextUnformatted(data.string().c_str());
			return false;
		}

		if (ImGui::InputText("##", &data_str, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			data = data_str;
			changed |= true;
		}

		ImGui::SameLine();

		auto files_var = GetMetaData(EPathMetaData_AsFiles);
		auto exts_var = GetMetaData(EPathMetaData_Extensions);

		auto as_files = files_var ? files_var.to_bool() : false;
		auto exts = exts_var ? exts_var.to_string() : ".*";

		if (ImGui::Button("...##"))
		{
			if (as_files)
			{
				if (auto info = Platform::OpenFile(exts.data()))
				{
					data = info;
					changed |= true;
				}
			}
			else if (auto info = Platform::GetDirectory())
			{
				data = info;
				changed |= true;
			}
		}

		if (changed)
		{
			var = data;
		}

		return changed;
	}

	REFLECT_INSPECTOR(Inspector_Path, std::filesystem::path)
} // namespace BHive
