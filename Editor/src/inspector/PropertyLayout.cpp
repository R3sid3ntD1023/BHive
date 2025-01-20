#include "gui/ImGuiExtended.h"
#include "inspector/PropertyLayout.h"

namespace BHive
{
	PropertyLayout::PropertyLayout(const std::string &name, bool columns, float width)
		: mColumns(columns),
		  mWidth(width),
		  mName(name)
	{
	}

	PropertyLayout::PropertyLayout(const rttr::property &property, bool columns, float width)
		: mColumns(columns),
		  mWidth(width)
	{
		std::string name = property.get_name().data();
		if (auto name_var = property.get_metadata(PROPERTY_PRETTY_NAME))
		{
			name = name_var.to_string();
		}

		mName = name;
	}

	PropertyLayout::~PropertyLayout()
	{
	}

	void PropertyLayout::PushLayout()
	{
		auto size = ImGui::GetContentRegionAvail();
		bool use_custom_width = mWidth != 0.f;

		if (mColumns)
		{
			ImGui::BeginTable((mName + "Property").c_str(), 2, 0, {use_custom_width ? mWidth : size.x, 0.f});
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
		}

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(mName.c_str());

		if (mColumns)
		{
			ImGui::TableNextColumn();
		}

		ImGui::PushID(mName.c_str());

		auto width = ImGui::GetContentRegionAvail().x;
		ImGui::PushItemWidth(use_custom_width ? mWidth : width);
	}

	void PropertyLayout::PopLayout()
	{
		// DEBUG_DRAW_RECT(0xFFFF0000)

		ImGui::PopID();
		ImGui::PopItemWidth();

		if (mColumns)
		{
			ImGui::EndTable();

			// DEBUG_DRAW_RECT(0xFF0000FF)
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