#pragma once

#include "asset/Asset.h"

namespace BHive
{
	class Texture;
	class EditorAssetManager;
	class Project;

	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel() = default;
		ContentBrowserPanel(const std::filesystem::path &directory);

		void OnGuiRender();

		void SetBaseDirectory(const std::filesystem::path &directory);
		const std::filesystem::path &CurrentDirectory() const { return mCurrentDirectory; }

		virtual void OnImportAsset(const std::filesystem::path &directory, const std::filesystem::path &relative) = 0;
		virtual void OnDeleteAsset(const std::filesystem::path &relative) = 0;
		virtual void OnRenameAsset(const std::filesystem::path &relative_old, const std::filesystem::path &relative_new, bool directory) = 0;
		virtual void OnReimportAsset(const std::filesystem::path &relative) {};
		virtual void OnAssetContextMenu(const std::filesystem::path &relative) {}
		virtual void OnAssetDoubleClicked(const std::filesystem::path &relative) {}
		virtual bool IsAssetValid(const std::filesystem::path &relative) const = 0;

		virtual Ref<Texture> OnGetIcon(bool directory, const std::filesystem::path &relative) = 0;
		virtual bool GetDragDropData(AssetHandle &data, const std::filesystem::path &relative) = 0;

	protected:
		virtual void OnWindowContextMenu() {};

	private:
		void ShowFileSystemTree(const std::filesystem::directory_entry &directory);
		void ShowFileSystem();
		void OnDeleteFolder(const std::filesystem::directory_entry &entry);
		void DeleteFolder(const std::filesystem::directory_entry &entry);
		void SetCurrentDirectory(const std::filesystem::path &path);

	private:
		std::filesystem::path mBaseDirectory;
		std::filesystem::path mCurrentDirectory;

		float mPadding = 16.f, mThumbnailSize = 128.f;
		bool mIsMouseDragging = false, mDragStarting = false;
	};
} // namespace BHive