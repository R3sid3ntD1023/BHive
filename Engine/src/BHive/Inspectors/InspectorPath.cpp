#include "InspectorPath.h"
#include "gui/ImGuiExtended.h"
#include "core/FileDialog.h"

namespace BHive
{
	bool
	Inspector_Path::Inspect(const rttr::variant &instance, rttr::variant &var, bool read_only, const meta_getter &metadata)
	{
		auto data = var.get_value<std::filesystem::path>();
		auto data_str = data.string();
		bool changed = false;

		if (read_only)
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

		if (ImGui::Button("...##"))
		{
			auto path_str = FileDialogs::GetDirectory();
			if (!path_str.empty())
			{
				data = path_str;
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
