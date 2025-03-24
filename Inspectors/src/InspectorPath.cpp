#include "InspectorPath.h"
#include "gui/ImGuiExtended.h"

namespace BHive
{
	bool
	Inspector_Path::Inspect(const rttr::variant &instance, rttr::variant &var, bool read_only, const meta_getter &metadata)
	{
		auto data = var.get_value<std::filesystem::path>();
		auto data_str = data.string();

		if (read_only)
		{
			ImGui::TextUnformatted(data.string().c_str());
			return false;
		}

		if (ImGui::InputText("##", &data_str, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			data = data_str;
			var = data;
			return true;
		}

		return false;
	}

	REFLECT_INSPECTOR(Inspector_Path, std::filesystem::path)
} // namespace BHive
