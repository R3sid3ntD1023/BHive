#pragma once

#include "renderers/SceneRenderer.h"
#include "core/EventDelegate.h"

namespace BHive
{
	class PickerRenderPass;

	DECLARE_EVENT(FOnEntitySelected, int32_t)

	class EditorSceneRenderer : public SceneRenderer
	{
	public:
		void Init(const glm::uvec2 &size) override;

		void End() override;

		void Pick(const glm::uvec2 &mouse_pos);

		void OnEntityPicked(int32_t i, const Ref<FMeshRenderData> &render_data);

		FOnEntitySelectedEvent OnEntitySelectedEvent{};

	private:
		Ref<PickerRenderPass> mPickPass{};
	};
} // namespace BHive