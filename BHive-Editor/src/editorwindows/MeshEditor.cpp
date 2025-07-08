#include "MeshEditor.h"
#include "mesh/StaticMesh.h"
#include "mesh/SkeletalMesh.h"
#include "mesh/Skeleton.h"
#include "mesh/SkeletalPose.h"
#include "renderers/SceneRenderer.h"

namespace BHive
{
	MeshEditor::MeshEditor()
	{
		mCamera = EditorCamera(45.0f, 1.0f, .01f, 1000.f);
		mCamera.SetView(FTransform({0, 3, 10}, {-45.f, 0.f, 0.f}));

		mSceneRenderer = CreateRef<SceneRenderer>();
		mSceneRenderer->Initialize(300, 300);
	}

	void MeshEditor::OnWindowRender()
	{
		mCamera.ProcessInput();

		if ((mViewportPanelSize.x != mViewportSize.x || mViewportPanelSize.y != mViewportSize.y) &&
			mViewportPanelSize.x != 0 && mViewportPanelSize.y != 0)
		{
			mViewportSize = mViewportPanelSize;
			mCamera.Resize(mViewportSize.x, mViewportSize.y);
			mSceneRenderer->Resize((unsigned)mViewportPanelSize.x, (unsigned)mViewportPanelSize.y);
		}

		mSceneRenderer->Begin(&mCamera, mCamera.GetView());

		LineRenderer::DrawGrid({});
		Renderer::SubmitDirectionalLight({0, 0, -1}, DirectionalLight{});

		if (auto skeletal_mesh = Cast<SkeletalMesh>(mAsset))
		{
			auto pose = skeletal_mesh->GetDefaultPose();
			MeshRenderer::DrawMesh(skeletal_mesh, *pose, FTransform());
		}
		else if (auto static_mesh = Cast<StaticMesh>(mAsset))
		{
			MeshRenderer::DrawMesh(static_mesh, FTransform());
		}

		mSceneRenderer->End();

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

		TAssetEditor::OnWindowRender();

		ImGui::EndTable();
	}

	void MeshEditor::OnEvent(Event &event)
	{
		mCamera.OnEvent(event);
	}
} // namespace BHive