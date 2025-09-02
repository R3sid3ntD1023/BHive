#pragma once

#include "asset/AssetFactory.h"
#include "asset/AssetManager.h"
#include "asset/EditorAssetManager.h"
#include "AssetEditor.h"
#include "inspectors/Inspect.h"
#include "project/Project.h"

namespace BHive
{
	template <typename T>
	class TAssetEditor : public AssetEditor
	{
	public:
		TAssetEditor(int flags = 0)
			: AssetEditor(flags)
		{
		}

		void SetContext(const UUID &handle) override
		{
			auto manager = AssetManager::GetAssetManager<EditorAssetManager>();
			auto metadata = manager->GetMetaData(handle);
			auto asset = AssetManager::GetAsset<T>(handle);

			mAsset = asset;

			if (mAsset)
			{
				mCurrentSavePath = Project::GetResourceDirectory() / metadata.Path;
				mLabel = metadata.Name;
				OnSetContext(mAsset);
			}
		}

		virtual void OnSetContext(const Ref<T> &asset) {}

		virtual void OnUpdateContent() override
		{
			if (!mAsset)
				return;

			/*auto edited_asset_override = GetEditedAssetOverride();

			auto ptr = edited_asset_override ? edited_asset_override.get() : mAsset.get();*/

			auto ptr = mAsset.get();
			rttr::variant var = ptr;
			if (Inspect::get().inspect("", this, var, true))
			{
				ptr = var.get_value<T *>();
			}
		}

		virtual const char *GetName() const override { return mLabel.c_str(); };

		virtual bool OnSave(const std::filesystem::path &path)
		{
			AssetFactory factory;
			return factory.Export(mAsset, path);
		}

		// virtual Ref<T> GetEditedAssetOverride() const { return nullptr; }

	protected:
		Ref<T> mAsset;
		std::string mLabel;
	};

} // namespace BHive
