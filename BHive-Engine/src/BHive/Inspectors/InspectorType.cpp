#include "InspectorType.h"
#include "gui/ImGuiExtended.h"
#include <rttr/type.h>

namespace BHive
{
	bool Inspector_Type::inspect(const rttr::variant &owner, rttr::variant &var, const MetaGetter &GetMetaData, const bool is_read_only)
	{
		auto data = var.get_value<rttr::type>();
		auto name = data.get_name();

		ImGui::TextUnformatted(name.data());
		return false;
	}

	REFLECT_INSPECTOR(Inspector_Type, rttr::type);
} // namespace BHive
