#include "mesh/SkeletalMesh.h"
#include "mesh/SkeletalPose.h"
#include "mesh/Skeleton.h"
#include "mesh/StaticMesh.h"
#include "MeshEditor.h"
#include "renderers/SceneRenderer.h"

namespace BHive
{
	MeshEditor::MeshEditor()
	{
		mCamera = EditorCamera(45.0f, 1.0f, .01f, 1000.f);

		mSceneRenderer = CreateRef<SceneRenderer>();
		mSceneRenderer->Initialize(300, 300);
	}

	void MeshEditor::OnUpdateContent()
	{
		mCamera.ProcessInput();

		if ((mViewportPanelSize.x != mViewportSize.x || mViewportPanelSize.y != mViewportSize.y) && mViewportPanelSize.x != 0 && mViewportPanelSize.y != 0)
		{
			mViewportSize = mViewportPanelSize;
			mCamera.Resize(mViewportSize.x, mViewportSize.y);
			mSceneRenderer->Resize((unsigned)mViewportPanelSize.x, (unsigned)mViewportPanelSize.y);
		}

		mSceneRenderer->Begin(&mCamera, mCamera.GetView());
		mSceneRenderer->SubmitCommand([]() { LineRenderer::DrawGrid(FGrid{.color = 0xffffffff, .stepcolor = 0xffffffff}); });
		mSceneRenderer->SubmitLight(FDirectionalLightCreateInfo{.Color = {1, 1, 1}, .Direction = {0, 0, -1}});

		FMeshInfo info{};
		if (auto skeletal_mesh = Cast<SkeletalMesh>(mAsset))
		{
			info.Mesh = skeletal_mesh;
			info.Materials = skeletal_mesh->GetMaterialTable();
			info.BoneInfo = CreateRef<FBoneInfo>();
			info.BoneInfo->Bones = skeletal_mesh->GetDefaultPose()->GetTransformsJointSpace();
		}
		else if (auto static_mesh = Cast<StaticMesh>(mAsset))
		{
			info.Mesh = static_mesh;
			info.Materials = static_mesh->GetMaterialTable();
		}

		mSceneRenderer->SubmitMesh(info);

		mSceneRenderer->End();

		// imgui
		ImGui::BeginTable("##content", 2, ImGuiTableFlags_Resizable);
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

	void MeshEditor::OnEvent(Event &event)
	{
		mCamera.OnEvent(event);
	}
} // namespace BHive