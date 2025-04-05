#pragma once

#include "asset/Asset.h"
#include "gui/ImGuiExtended.h"
#include "WindowBase.h"

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

	class ContentBrowserPanel : public WindowBase
	{
	public:
		ContentBrowserPanel() = default;
		ContentBrowserPanel(const std::filesystem::path &directory);

		void OnGuiRender();

		void SetBaseDirectory(const std::filesystem::path &directory);
		const std::filesystem::path &CurrentDirectory() const { return mCurrentDirectory; }

		virtual void OnImportAsset(const std::filesystem::path &directory, const std::filesystem::path &relative) {};
		virtual void OnDeleteAsset(const std::filesystem::path &relative) {};
		virtual void
		OnRenameAsset(const std::filesystem::path &relative_old, const std::filesystem::path &relative_new, bool directory) {
		};
		virtual void OnReimportAsset(const std::filesystem::path &relative) {};
		virtual void OnAssetContextMenu(const std::filesystem::path &relative) {}
		virtual void OnAssetDoubleClicked(const std::filesystem::path &relative) {}
		virtual bool IsAssetValid(const std::filesystem::path &relative) const { return false; };

		virtual Ref<Texture2D> OnGetIcon(bool directory, const std::filesystem::path &relative) { return nullptr; };
		virtual bool GetDragDropData(UUID &data, const std::filesystem::path &relative) { return false; };

	protected:
		virtual void OnWindowContextMenu() {};
		virtual const char *GetName() const { return "Content Browser"; }

	private:
		void ShowFileSystemTree(const std::filesystem::directory_entry &directory);
		void ShowFileSystem();
		void OnDeleteFolder(const std::filesystem::directory_entry &entry);
		void DeleteFolder(const std::filesystem::directory_entry &entry);
		void SetCurrentDirectory(const std::filesystem::path &path);

	private:
		std::filesystem::path mBaseDirectory;
		std::filesystem::path mCurrentDirectory;

		float mPadding = 16.f, mThumbnailSize = 90.f;
		bool mIsMouseDragging = false, mDragStarting = false;
		FContentBrowserStyle mStyle{};
	};
} // namespace BHive