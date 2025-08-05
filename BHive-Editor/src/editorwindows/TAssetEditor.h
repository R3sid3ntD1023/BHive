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

			auto ptr = mAsset.get();
			rttr::variant var = ptr;
			if (Inspect::get().inspect("", this, var, true))
			{
				ptr = var.get_value<T *>();
			}
		}

		virtual const char *GetName() const override { return mLabel.c_str(); };

		bool OnSave(const std::filesystem::path &path) const
		{
			AssetFactory factory;
			return factory.Export(mAsset, path);
		}

	protected:
		Ref<T> mAsset;
		std::string mLabel;
	};

} // namespace BHive
