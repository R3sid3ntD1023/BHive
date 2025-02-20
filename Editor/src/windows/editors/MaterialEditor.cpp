#include "MaterialEditor.h"
#include "mesh/primitives/Sphere.h"
#include "scene/SceneRenderer.h"

namespace BHive
{
	MaterialEditor::MaterialEditor()
	{
		if (!mSphere)
		{
			mSphere = CreateRef<PSphere>(1.f, 64, 64);
		}

		mCamera = glm::perspective(45.0f, 1.f, 0.01f, 1000.f);
		mRenderer = CreateRef<SceneRenderer>(glm::ivec2{300, 300}, ESceneRendererFlags_NoShadows);
	}

	void MaterialEditor::OnWindowRender()
	{
		if ((mViewportPanelSize.x != mViewportSize.x || mViewportPanelSize.y != mViewportSize.y) &&
			mViewportPanelSize.x != 0 && mViewportPanelSize.y != 0)
		{
			mViewportSize = mViewportPanelSize;
			mCamera = glm::perspective(45.0f, mViewportSize.x / mViewportSize.y, 0.01f, 1000.f);
			mRenderer->Resize((unsigned)mViewportSize.x, (unsigned)mViewportSize.y);
		}

		mRenderer->Begin(mCamera.GetProjection(), glm::inverse(glm::translate(glm::vec3{0, 0, 3.f})));
		mRenderer->SubmitLight(DirectionalLight{}, FTransform({}, {0, -180.f, 0.f}));
		mRenderer->SubmitStaticMesh(mSphere, FTransform(), mAsset);
		mRenderer->End();

		ImGui::BeginTable("##content", 2);
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
} // namespace BHive