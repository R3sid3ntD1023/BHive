#include "EditorSceneRenderer.h"
#include "core/subsystem/SubSystem.h"
#include "gfx/Framebuffer.h"
#include "runtime/GameObject.h"
#include "subsystems/Selection.h"
#include <imgui.h>

namespace BHive
{
	void EditorSceneRenderer::Init(const glm::uvec2 &size)
	{
		SceneRenderer::Init(size);

		// mPickPass = PushRenderPass<PickerRenderPass>();
		// mPickPass->OnEntityPicked.bind(this, &EditorSceneRenderer::OnEntityPicked);
	}

	void EditorSceneRenderer::End()
	{

		SceneRenderer::End();
	}

	void EditorSceneRenderer::Pick(const glm::uvec2 &mouse_pos)
	{
		// mPickPass->Pick(mouse_pos);
	}

	// void EditorSceneRenderer::OnEntityPicked(int32_t i, const Ref<FMeshRenderData> &render_data)
	// {
	// 	OnEntitySelectedEvent.invoke(i);
	// }
} // namespace BHive