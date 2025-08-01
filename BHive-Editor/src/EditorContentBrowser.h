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

		virtual void OnImportAsset(const std::filesystem::path &directory, const std::filesystem::path &relative) override;
		virtual void OnDeleteAsset(const std::filesystem::path &relative) override;
		virtual void OnRenameAsset(const std::filesystem::path &relative_old, const std::filesystem::path &relative_new) override;
		virtual void OnReimportAsset(const std::filesystem::path &relative) override;
		virtual void OnAssetContextMenu(const std::filesystem::path &relative) override;
		virtual void OnAssetDoubleClicked(const std::filesystem::path &relative) override;
		virtual bool IsAssetValid(const std::filesystem::path &relative) const override;

		virtual Ref<Texture2D> OnGetIcon(const std::filesystem::directory_entry &entry) override;
		virtual bool GetDragDropData(UUID &data, const std::filesystem::path &relative) override;
		virtual void OnCreateAssetMenu() override;

	private:
		virtual void OnCreateAsset(const std::filesystem::path &directory, const Ref<Factory> &factory);
	};

} // namespace BHive

#include "EditorContentBrowser.inl"