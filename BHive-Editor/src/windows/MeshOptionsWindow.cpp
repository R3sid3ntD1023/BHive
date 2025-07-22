#include "asset/AssetManager.h"
#include "asset/EditorAssetManager.h"
#include "core/FileDialog.h"
#include "factories/MaterialFactory.h"
#include "factories/MeshFactory.h"
#include "factories/TextureFactory.h"
#include "gui/ImGuiExtended.h"
#include "inspectors/Inspect.h"
#include "MeshOptionsWindow.h"

namespace BHive
{
	MeshOptionsWindow::MeshOptionsWindow(
		MeshFactory *factory, const std::filesystem::path &filePath, const FMeshImportData &data)
		: WindowBase(ImGuiWindowFlags_NoSavedSettings),
		  mImportData(data),
		  mFactory(factory),
		  mImportPath(filePath)
	{
		mOptions.MeshType = data.mBoneData.size() ? EMeshType::SkeletalMesh : EMeshType::StaticMesh;
		mOptions.AssetPath = filePath;
		mOptions.OverideMaterials.resize(data.mMaterialData.size());
	}

	void MeshOptionsWindow::OnGuiRender()
	{
		bool animations_enabled = mImportData.mBoneData.size() > 0 && mOptions.MeshType != EMeshType::StaticMesh;

		if (Inspect::inspect("Mesh Type", mOptions.MeshType))
		{
			if (mOptions.MeshType == EMeshType::SkeletalAnimation)
			{
				mOptions.ImportAnimations = true;
			}
		}

		ImGui::SeparatorText("Materials");

		ImGui::BeginDisabled(!mImportData.mMaterialData.size());
		Inspect::inspect("Import Materials", mOptions.ImportMaterials);
		ImGui::EndDisabled();

		// allow overriding materials
		ImGui::SeparatorText("Override Materials");
		for (size_t i = 0; i < mOptions.OverideMaterials.size(); i++)
		{
			Inspect::inspect(std::format("Material {}", i), mOptions.OverideMaterials[i]);
		}

		ImGui::SeparatorText("Animations");
		ImGui::BeginDisabled(!animations_enabled);

		Inspect::inspect("Import Animations", mOptions.ImportAnimations);
		Inspect::inspect("Skeleton", mOptions.Skeleton);

		ImGui::EndDisabled();

		if (ImGui::Button("Cancel"))
		{
			mShouldClose = true;
		}

		if (ImGui::Button("Import"))
		{
			auto asset_manager = AssetManager::GetAssetManager<EditorAssetManager>();
			if (!asset_manager)
			{
				return;
			}

			auto load_texture = [](const std::filesystem::path &path) -> Ref<Texture>
			{
				TextureFactory factory;
				return Cast<Texture>(factory.Import(path));
			};
			auto load_texture_memory = [](const uint8_t *data, size_t size) -> Ref<Texture>
			{
				TextureFactory factory;
				return Cast<Texture>(factory.Import(data, size));
			};
			auto create_material = []() -> Ref<Material>
			{
				MaterialFactory factory;
				return Cast<Material>(factory.CreateNew());
			};

			std::vector<Ref<Asset>> other_assets;
			MeshImportResolver resolver(mImportData, mOptions, other_assets);
			resolver.SetLoaders(load_texture, load_texture_memory, create_material);
			Ref<Asset> asset = resolver.Resolve();

			if (asset)
			{
				if (other_assets.size())
					mFactory->mOtherAssets = other_assets;

				mFactory->OnImportCompleted.invoke(asset);
			}
		}
	}

	REFLECT(EMeshType)
	{
		BEGIN_REFLECT_ENUM(EMeshType)(ENUM_VALUE(StaticMesh), ENUM_VALUE(SkeletalMesh), ENUM_VALUE(SkeletalAnimation));
	}
} // namespace BHive
