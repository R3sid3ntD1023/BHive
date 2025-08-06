#pragma once

#include "windows/ContentBrowserWindow.h"

namespace BHive
{
	class Factory;

	template <typename T>
	class EditorContentBrowser : public ImContentBrowserWindow
	{

	public:
		EditorContentBrowser(const std::filesystem::path &directory);

		virtual void OnImportAsset(const std::filesystem::path &dir, const std::filesystem::path &path) override;
		virtual void OnDeleteAsset(const std::filesystem::path &path) override;
		virtual void OnRenameAsset(const std::filesystem::path &_old, const std::filesystem::path &_new) override;
		virtual void OnReimportAsset(const std::filesystem::path &path) override;
		virtual void OnAssetContextMenu(const std::filesystem::path &path) override;
		virtual void OnAssetDoubleClicked(const std::filesystem::path &path) override;
		virtual bool IsAssetValid(const std::filesystem::path &path) const override;

		virtual Ref<Texture2D> OnGetIcon(const std::filesystem::path &path) override;
		virtual void OnCreateAssetMenu() override;

	private:
		virtual void OnCreateAsset(const std::filesystem::path &directory, const Ref<Factory> &factory);
		void RenameAsset(const std::filesystem::path &_old, const std::filesystem::path &_new, const std::filesystem::recursive_directory_iterator &it);
	};

} // namespace BHive

#include "EditorContentBrowser.inl"