#pragma once

#include "core/Core.h"
#include "core/EventDelegate.h"
#include "RenderPass.h"

namespace BHive
{
	class Framebuffer;
	class Shader;

	DECLARE_EVENT(FOnEntityPicked, int32_t, const Ref<FMeshRenderData> &);

	class BHIVE_API PickerRenderPass : public RenderPass
	{
	public:
		void Init() override;

		void Render(const FMeshRenderDatas &data);

		void Pick(const glm::uvec2 mousePos);

		void CreateFramebuffer() override;

		bool IsEnabled() const { return RenderPass::IsEnabled() && mEnabled; }

		FOnEntityPickedEvent OnEntityPicked;

	private:
		bool mEnabled{false};
		Ref<Shader> mShaders[3];
		glm::uvec2 mMousePos{0, 0};
	};
} // namespace BHive