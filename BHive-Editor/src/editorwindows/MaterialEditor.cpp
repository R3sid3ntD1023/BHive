#include "MaterialEditor.h"
#include "mesh/primitives/Sphere.h"
#include "renderers/Renderer.h"
#include "renderers/SceneRenderer.h"

namespace BHive
{
	MaterialEditor::MaterialEditor()
	{
		if (!mSphere)
		{
			mSphere = CreateRef<PSphere>(1.f, 64, 64);
			mSphere->GetMaterialTable().resize(1);
		}

		mCamera = EditorCamera(45.0f, 1.f, 0.01f, 1000.f);

		mSceneRenderer = CreateRef<SceneRenderer>();
		mSceneRenderer->Init({300, 300});
	}

	void MaterialEditor::OnUpdateContent()
	{
		mCamera.ProcessInput();

		if ((mViewportPanelSize.x != mViewportSize.x || mViewportPanelSize.y != mViewportSize.y) && mViewportPanelSize.x != 0 && mViewportPanelSize.y != 0)
		{
			mSceneRenderer->Resize(mViewportPanelSize);
			mViewportSize = mViewportPanelSize;
			mCamera.Resize(mViewportSize.x, mViewportSize.y);
		}

		FMeshInfo info{};
		info.Mesh = mSphere;
		info.Materials = mSphere->GetMaterialTable();

		mSceneRenderer->Begin(&mCamera, mCamera.GetView());
		mSceneRenderer->SubmitLight(FDirectionalLightCreateInfo{.Color = {1, 1, 1}, .Direction = {-1, -.5, 0}});
		mSceneRenderer->SubmitMesh(info);
		mSceneRenderer->SubmitCommand([]() { LineRenderer::DrawGrid(FGrid{.color = 0xffffffff, .stepcolor = 0xffffffff}); });
		mSceneRenderer->End();

		// imgui
		ImGui::BeginTable("##content", 2);
		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		auto texture = mSceneRenderer->GetColorAttachment();

		if (ImGui::BeginChild("##viewer", {0, 0}, ImGuiChildFlags_ResizeY))
		{
			auto size = ImGui::GetContentRegionAvail();

			if (texture)
				ImGui::Image((ImTextureID)(uint64_t)(uint32_t)*texture, size, {0, 1}, {1, 0});

			mViewportPanelSize = {size.x, size.y};
		}

		ImGui::EndChild();

		ImGui::TableNextColumn();

		TAssetEditor::OnUpdateContent();

		ImGui::EndTable();
	}

	void MaterialEditor::OnSetContext(const Ref<Material> &asset)
	{
		/*	mTempMaterialAsset = asset->Clone();*/
		mSphere->GetMaterialTable().set_material(asset);
	}

	/*bool MaterialEditor::OnSave(const std::filesystem::path &path)
	{
		auto original_ptr = mAsset.get();
		*original_ptr = *mTempMaterialAsset.get();
		return TAssetEditor::OnSave(path);
	}

	Ref<Material> MaterialEditor::GetEditedAssetOverride() const
	{
		return mTempMaterialAsset;
	}*/
} // namespace BHive