#include "EditorSceneRenderer.h"
#include "renderers/render_passes/PickerRenderPass.h"
#include "renderers/render_passes/OutlineRenderPass.h"
#include "subsystems/Selection.h"
#include "core/subsystem/SubSystem.h"
#include "world/GameObject.h"
#include "gfx/Framebuffer.h"
#include <imgui.h>

namespace BHive
{
	void EditorSceneRenderer::Init(const glm::uvec2 &size)
	{
		SceneRenderer::Init(size);

		mPickPass = PushRenderPass<PickerRenderPass>();
		mPickPass->OnEntityPicked.bind(this, &EditorSceneRenderer::OnEntityPicked);

		mOutlinePass = PushRenderPass<OutlineRenderPass>();
		mOutlinePostProcess = PushPostProcessRenderPass<OutlinePostProcessRenderPass>();
	}

	void EditorSceneRenderer::End()
	{
		ASSERT(mOutlinePass && mOutlinePostProcess && mPickPass);

		auto &selection = GetSubSystem<Selection>();
		if (auto object = selection.GetSelection(); object && mSelectedRenderData)
			mSelectedRenderData->Transform = object->GetWorldTransform();

		mOutlinePass->SetSelected(mSelectedRenderData);
		mOutlinePostProcess->SetSelected(mSelectedRenderData != nullptr);
		mOutlinePostProcess->SetOutlineTexture(mOutlinePass->GetOutputTetxure());

		SceneRenderer::End();
	}

	void EditorSceneRenderer::Pick(const glm::uvec2 &mouse_pos)
	{
		mPickPass->Pick(mouse_pos);
	}

	void EditorSceneRenderer::OnEntityPicked(int32_t i, const Ref<FMeshRenderData> &render_data)
	{
		OnEntitySelectedEvent.invoke(i);
		mSelectedRenderData = render_data;
	}

	void EditorSceneRenderer::ClearPicked()
	{
		mSelectedRenderData = nullptr;
	}

	void EditorSceneRenderer::OnImGuiRender()
	{
		auto pick_attachment = mPickPass->GetFramebuffer()->GetColorAttachment();
		auto outline_attachment = mOutlinePass->GetFramebuffer()->GetColorAttachment();

		if (ImGui::Begin("Pick & Outline Pass"))
		{
			auto pick_tex = (ImTextureID)(uint64_t)(uint32_t)*pick_attachment;
			auto outline_tex = (ImTextureID)(uint64_t)(uint32_t)*outline_attachment;

			ImGui::Image(pick_tex, {400, 400}, {0, 1}, {1, 0});
			ImGui::Image(outline_tex, {400, 400}, {0, 1}, {1, 0});
		}

		ImGui::End();
	}
} // namespace BHive