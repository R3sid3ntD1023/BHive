#pragma once

#include "asset/AssetFactory.h"
#include "asset/AssetManager.h"
#include "asset/EditorAssetManager.h"
#include "AssetEditor.h"
#include "Inspectors.h"
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

		void SetContext(const UUID &handle)
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

		virtual void OnWindowRender()
		{
			rttr::variant var = mAsset.get();
			rttr::variant instance = this;
			if (inspect(instance, var, true))
			{
				auto ptr = mAsset.get();
				ptr = var.get_value<T*>();
			}
		}

		virtual const char *GetName() const { return mLabel.c_str(); };

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
