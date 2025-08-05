#pragma once

#include "core/Core.h"
#include "gui/GUICore.h"

namespace BHive
{
	struct ImTreeFolder
	{
		ImTreeFolder() = default;
		ImTreeFolder(const std::filesystem::directory_entry &entry);

		bool Draw(ImTextureID texture, float width, ImU32 icon_color, ImU32 icon_hovered_color);

		inline const ImGuiID &GetID() const { return mID; }

		const std::filesystem::directory_entry &GetEntry() const { return mEntry; }

		void AddChild(const ImTreeFolder &child);

		std::vector<ImTreeFolder> &GetChildren() { return mChildren; }

	private:
		ImGuiID mID = 0;
		std::filesystem::directory_entry mEntry;
		std::vector<ImTreeFolder> mChildren;
	};

	struct ImDirectoryEntry
	{
		using EntryEventCallback = std::function<void(const std::filesystem::directory_entry &, bool directory)>;
		using EntryDragDropCallback = std::function<void(const std::filesystem::directory_entry &)>;
		using EntryRenamedCallback = std::function<void(const std::filesystem::path &_old, const std::filesystem::path &_new)>;

		ImDirectoryEntry(const std::filesystem::directory_entry &entry);

		void SetDoubleClickedCallback(const EntryEventCallback &e);

		void SetDragDropSourceCallback(const EntryDragDropCallback &e);

		void SetDragDropTargetCallback(const EntryDragDropCallback &e);

		void SetRenamedCallback(const EntryRenamedCallback &e);

		void SetContextMenuCallback(const EntryEventCallback &e);

		bool IsDirectory() const;

		bool Draw(ImTextureID icon, const ImVec2 &size, bool selected, bool show_checkmark, ImU32 image_color);

		inline const ImGuiID &GetID() const { return mID; }

		const std::filesystem::directory_entry &GetEntry() const { return mEntry; }

	private:
		ImGuiID mID = 0;
		std::filesystem::directory_entry mEntry;
		EntryEventCallback mOnDoubleClicked;
		EntryDragDropCallback mOnDragDropSource;
		EntryDragDropCallback mOnDragDropTarget;
		EntryRenamedCallback mOnRenamed;
		EntryEventCallback mOnContextMenu;
	};
} // namespace BHive