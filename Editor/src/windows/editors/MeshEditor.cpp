#include "MeshEditor.h"
#include "scene/SceneRenderer.h"
#include "mesh/SkeletalMesh.h"
#include "mesh/Skeleton.h"
#include "mesh/SkeletalPose.h"

namespace BHive
{
	MeshEditor::MeshEditor()
	{
		mCamera = EditorCamera(45.0f, 1.0f, .01f, 1000.f);
		mRenderer = CreateRef<SceneRenderer>(glm::ivec2{300, 300}, ESceneRendererFlags_NoShadows);
	}

	void MeshEditor::OnWindowRender()
	{
		mCamera.ProcessInput();

		if ((mViewportPanelSize.x != mViewportSize.x || mViewportPanelSize.y != mViewportSize.y) &&
			mViewportPanelSize.x != 0 && mViewportPanelSize.y != 0)
		{
			mViewportSize = mViewportPanelSize;
			mCamera.Resize(mViewportSize.x, mViewportSize.y);
			mRenderer->Resize((unsigned)mViewportSize.x, (unsigned)mViewportSize.y);
		}

		mRenderer->Begin(mCamera.GetProjection(), mCamera.GetView().inverse());
		mRenderer->SubmitLight(DirectionalLight{}, FTransform({}, {0, -180.f, 0.f}));

		if (auto skeletal_mesh = Cast<SkeletalMesh>(mAsset))
		{
			auto &joints = skeletal_mesh->GetSkeleton()->GetRestPoseTransforms();
			mRenderer->SubmitSkeletalMesh(skeletal_mesh, FTransform(), joints, skeletal_mesh->GetMaterialTable());
		}
		else if (auto static_mesh = Cast<StaticMesh>(mAsset))
		{
			mRenderer->SubmitStaticMesh(static_mesh, FTransform(), static_mesh->GetMaterialTable());
		}

		mRenderer->End();

		ImGui::BeginTable("##content", 2, ImGuiTableFlags_Resizable);
		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		auto texture = mRenderer->GetFinalFramebuffer()->GetColorAttachment();

		if (ImGui::BeginChild("##viewer", {0, 0}, ImGuiChildFlags_ResizeY))
		{
			auto size = ImGui::GetContentRegionAvail();

			if (texture)
				ImGui::Image((ImTextureID)(uint64_t)(uint32_t)*texture, size, {0, 1}, {1, 0});

			mViewportPanelSize = {size.x, size.y};
		}

		ImGui::EndChild();

		ImGui::TableNextColumn();

		TAssetEditor::OnWindowRender();

		ImGui::EndTable();
	}

	void MeshEditor::OnEvent(Event &event)
	{
		mCamera.OnEvent(event);
	}
} // namespace BHive