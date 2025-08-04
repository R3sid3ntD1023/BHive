#pragma once

#include "asset/Asset.h"
#include "ImWindowBase.h"

#define DRAG_DROP_SOURCE_TYPE "CONTENT_BROWSER_ITEM"

namespace BHive
{
	class Texture2D;
	class Project;

	struct FContentBrowerColors
	{
		ImU32 mFolder{IM_COL32(255, 218, 84, 255)};
		ImU32 mFolderHovered{IM_COL32(255, 235, 166, 255)};
		ImU32 mSelection{IM_COL32(255, 100, 0, 255)};
		ImU32 mCheckMark{IM_COL32(180, 180, 255, 180)};
	};

	struct FContentBrowserStyle
	{
		FContentBrowerColors mColors;
	};

	class ImContentBrowserWindow : public ImWindowBase
	{
		using ContentBrowserActionFunc = std::function<void()>;

		struct FileEntry
		{
			ImGuiID ID;
			std::filesystem::directory_entry Entry;
		};

		using EntryItems = std::vector<FileEntry>;

	public:
		ImContentBrowserWindow();
		ImContentBrowserWindow(const std::filesystem::path &directory);

		void OnUpdate() override;

		void SetBaseDirectory(const std::filesystem::path &directory);
		const std::filesystem::path &CurrentDirectory() const { return mCurrentDirectory; }

		virtual void OnImportAsset(const std::filesystem::path &directory, const std::filesystem::path &relative) {};
		virtual void OnDeleteAsset(const std::filesystem::path &relative) {};
		virtual void OnRenameAsset(const std::filesystem::path &relative_old, const std::filesystem::path &relative_new) {};
		virtual void OnReimportAsset(const std::filesystem::path &relative) {};
		virtual void OnAssetContextMenu(const std::filesystem::path &relative) {}
		virtual void OnAssetDoubleClicked(const std::filesystem::path &relative) {}
		virtual bool IsAssetValid(const std::filesystem::path &relative) const { return false; };

		virtual Ref<Texture2D> OnGetIcon(const std::filesystem::directory_entry &entry) { return nullptr; };

	protected:
		virtual void OnCreateAssetMenu();
		virtual const char *GetName() const { return "Content Browser"; }

	private:
		void ShowFileSystemTree(const std::filesystem::directory_entry &directory);
		void ShowFileSystem();
		void OnDeleteFolder(const std::filesystem::directory_entry &entry);
		void DeleteFolder(const std::filesystem::directory_entry &entry);
		void SetCurrentDirectory(const std::filesystem::path &path);
		void ApplyDragDropTarget(const std::filesystem::directory_entry &entry);

	private:
		std::filesystem::path mBaseDirectory;
		std::filesystem::path mCurrentDirectory;

		float mPadding = 16.f, mThumbnailSize = 90.f;
		FContentBrowserStyle mStyle{};

		// content browser actions
		ContentBrowserActionFunc mContentBrowerAction;

		ImGuiSelectionBasicStorage mSelection;
		EntryItems mItems;
	};
} // namespace BHive