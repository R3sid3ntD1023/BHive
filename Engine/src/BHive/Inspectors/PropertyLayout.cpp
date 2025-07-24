#include "gui/ImGuiExtended.h"
#include "PropertyLayout.h"

namespace BHive
{
	PropertyLayout::PropertyLayout(const std::string &name, bool columns, float width)
		: mColumns(columns && !name.empty()),
		  mWidth(width),
		  mName(name)
	{
	}

	PropertyLayout::PropertyLayout(const rttr::property &property, bool columns, float width)
		: mColumns(columns),
		  mWidth(width)
	{
		std::string name = property.get_name().data();
		if (auto name_var = property.get_metadata(EPropertyMetaData_DisplayName))
		{
			name = name_var.to_string();
		}

		mName = name;
		mColumns = mColumns && !mName.empty();
	}

	PropertyLayout::~PropertyLayout()
	{
	}

	void PropertyLayout::PushLayout()
	{
		float size = mWidth != 0.f ? mWidth : ImGui::GetContentRegionAvail().x;

		if (mColumns)
		{
			ImGui::BeginTable((mName + "Property").c_str(), 2, 0, {mWidth, 0.f});
			ImGui::TableNextRow();
			ImGui::TableNextColumn();

			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted(mName.c_str());

			ImGui::TableNextColumn();
		}

		ImGui::PushID(mName.c_str());
	}

	void PropertyLayout::PopLayout()
	{
		ImGui::PopID();

		if (mColumns)
		{
			ImGui::EndTable();
		}
	}

	ScopedPropertyLayout::ScopedPropertyLayout(const std::string &name, bool columns, float width)
		: PropertyLayout(name, columns, width)
	{
		PushLayout();
	}

	ScopedPropertyLayout::ScopedPropertyLayout(const rttr::property &property, bool columns, float width)
		: PropertyLayout(property, columns, width)
	{
		PushLayout();
	}

	ScopedPropertyLayout::~ScopedPropertyLayout()
	{
		PopLayout();
	}
} // namespace BHive