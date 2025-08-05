#include "ContentBrowserItems.h"
#include "gui/ImGuiExtended.h"

namespace BHive
{
	ImTreeFolder::ImTreeFolder(const std::filesystem::directory_entry &entry)
		: mEntry(entry),
		  mID(ImGuiID(entry.path().string().c_str()))
	{
	}

	bool ImTreeFolder::Draw(ImTextureID texture, float width, ImU32 icon_color, ImU32 icon_hovered_color)
	{
		auto label = mEntry.path().stem().string();

		ImVec2 size = {width, ImGui::GetFontSize()};

		bool pressed = ImGui::InvisibleButton("##icon", size, 0);

		auto rect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
		auto hovered = ImGui::IsItemHovered();
		auto active = ImGui::IsItemActive();
		auto color = hovered || active ? icon_hovered_color : icon_color;
		auto frame_color = hovered ? ImGui::GetColorU32(ImGuiCol_FrameBgHovered) : IM_COL32(0, 0, 0, 0);

		auto drawlist = ImGui::GetWindowDrawList();
		drawlist->AddRectFilled(rect.Min, rect.Max, frame_color);
		drawlist->AddImage(texture, rect.Min, rect.Min + ImVec2{size.y, size.y}, {0, 1}, {1, 0}, color);
		drawlist->AddText({rect.Min.x + size.y + GImGui->Style.FramePadding.x, rect.Min.y}, IM_COL32_WHITE, label.c_str());

		return pressed;
	}

	void ImTreeFolder::AddChild(const ImTreeFolder &child)
	{
		mChildren.push_back(child);
	}

	ImDirectoryEntry::ImDirectoryEntry(const std::filesystem::directory_entry &entry)
		: mEntry(entry)
	{
	}

	void ImDirectoryEntry::SetDoubleClickedCallback(const EntryEventCallback &e)
	{
		mOnDoubleClicked = e;
	}
	void ImDirectoryEntry::SetDragDropSourceCallback(const EntryDragDropCallback &e)
	{
		mOnDragDropSource = e;
	}
	void ImDirectoryEntry::SetDragDropTargetCallback(const EntryDragDropCallback &e)
	{
		mOnDragDropTarget = e;
	}
	void ImDirectoryEntry::SetRenamedCallback(const EntryRenamedCallback &e)
	{
		mOnRenamed = e;
	}
	void ImDirectoryEntry::SetContextMenuCallback(const EntryEventCallback &e)
	{
		mOnContextMenu = e;
	}

	bool ImDirectoryEntry::IsDirectory() const
	{
		return mEntry.is_directory();
	}

	bool ImDirectoryEntry::Draw(ImTextureID icon, const ImVec2 &size, bool selected, bool show_checkmark, ImU32 image_color)
	{
		mID = ImGui::GetID(mEntry.path().string().c_str());

		ImGui::PushID(mID);

		const bool directory = IsDirectory();
		const auto path = mEntry.path();
		const auto name = path.stem().string();

		bool clicked = ImGui::Selectable("", selected, ImGuiSelectableFlags_SelectOnClick | ImGuiSelectableFlags_AllowDoubleClick, size);

		auto rect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

		if (ImGui::BeginPopupContextItem(name.c_str()))
		{
			mOnContextMenu(mEntry, directory);

			ImGui::EndPopup();
		}

		auto drawlist = ImGui::GetWindowDrawList();
		auto color = directory ? image_color : IM_COL32_WHITE;
		drawlist->AddImage(icon, rect.Min, rect.Max, {0, 1}, {1, 0}, color);

		if (selected)
		{
			auto hightlight = ImGui::GetColorU32(ImGuiCol_NavHighlight);
			drawlist->AddRect(rect.Min, rect.Max, hightlight);
		}

		if (clicked)
		{
			if (ImGui::IsMouseDoubleClicked(0) && mOnDoubleClicked)
			{
				mOnDoubleClicked(mEntry, directory);
			}
		}

		if (directory)
		{
			if (ImGui::BeginDragDropTarget())
			{
				if (mOnDragDropTarget)
					mOnDragDropTarget(mEntry);

				ImGui::EndDragDropTarget();
			}
		}

		if (ImGui::BeginDragDropSource())
		{
			if (mOnDragDropSource)
				mOnDragDropSource(mEntry);

			ImGui::Image(icon, size);
			ImGui::TextUnformatted(name.c_str());

			ImGui::EndDragDropSource();
		}

		if (show_checkmark && !directory)
		{
			auto checkmark_size = 20.0f;
			auto color = ImGui::GetColorU32(ImGuiCol_CheckMark);
			ImGui::RenderCheckMark(drawlist, {rect.Max.x - checkmark_size, rect.Max.y - checkmark_size}, color, checkmark_size);
		}

		if (mOnRenamed)
		{
			std::string new_name;
			bool edited_name = ImGui::DrawEditableText(name, new_name);

			if (edited_name)
			{
				auto new_path = path.parent_path() / (new_name + path.extension().string());
				mOnRenamed(path, new_path);
			}
		}

		ImGui::PopID();

		return clicked;
	}
} // namespace BHive