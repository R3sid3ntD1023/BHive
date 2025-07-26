#pragma once

#include "ContentBrowser.h"

namespace BHive
{
	class Factory;

	template <typename T>
	class EditorContentBrowser : public ContentBrowserPanel
	{

	public:
		EditorContentBrowser(const std::filesystem::path &directory);

		virtual void OnImportAsset(const std::filesystem::path &directory, const std::filesystem::path &relative);
		virtual void OnDeleteAsset(const std::filesystem::path &relative);
		virtual void
		OnRenameAsset(const std::filesystem::path &relative_old, const std::filesystem::path &relative_new, bool directory);
		virtual void OnReimportAsset(const std::filesystem::path &relative) override;
		virtual void OnAssetContextMenu(const std::filesystem::path &relative);
		virtual void OnAssetDoubleClicked(const std::filesystem::path &relative);
		virtual bool IsAssetValid(const std::filesystem::path &relative) const;

		virtual Ref<Texture2D> OnGetIcon(bool directory, const std::filesystem::path &relative);
		virtual bool GetDragDropData(UUID &data, const std::filesystem::path &relative);
		virtual void OnCreateAssetMenu();

	private:
		virtual void OnCreateAsset(const std::filesystem::path &directory, const Ref<Factory> &factory);
	};

} // namespace BHive

#include "EditorContentBrowser.inl"