#include "MaterialEditor.h"
#include "mesh/primitives/Sphere.h"
#include "renderers/Renderer.h"
#include "gfx/Framebuffer.h"
#include "gfx/RenderCommand.h"
#include "renderers/SceneRenderer.h"
#include "renderers/postprocessing/Bloom.h"

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
		mCamera.SetView(FTransform({0, 3, 5}, {-45.f, 0.f, 0.f}));

		mSceneRenderer = CreateRef<SceneRenderer>();
		mSceneRenderer->Initialize(300, 300, SceneRendererFlags::RenderQuad | SceneRendererFlags::Bloom);
	}

	void MaterialEditor::OnWindowRender()
	{
		mCamera.ProcessInput();

		if ((mViewportPanelSize.x != mViewportSize.x || mViewportPanelSize.y != mViewportSize.y) &&
			mViewportPanelSize.x != 0 && mViewportPanelSize.y != 0)
		{
			mSceneRenderer->Resize((unsigned)mViewportPanelSize.x, (unsigned)mViewportPanelSize.y);
			mViewportSize = mViewportPanelSize;
			mCamera.Resize(mViewportSize.x, mViewportSize.y);
		}

		mSceneRenderer->Begin(&mCamera, mCamera.GetView());

		Renderer::SubmitDirectionalLight({-1, -.5, 0}, DirectionalLight{});
		MeshRenderer::DrawMesh(mSphere, FTransform());
		LineRenderer::DrawGrid(FGrid{.color = 0xffffffff, .stepcolor = 0xffffffff});

		mSceneRenderer->End();

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

		TAssetEditor::OnWindowRender();

		ImGui::EndTable();
	}

	void MaterialEditor::OnSetContext(const Ref<Material> &asset)
	{
		mSphere->GetMaterialTable().set_material(asset);
	}
} // namespace BHive