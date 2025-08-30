#pragma once

#include "core/Core.h"
#include "core/EventDelegate.h"
#include "RenderPass.h"

namespace BHive
{
	class Framebuffer;
	class Shader;

	DECLARE_EVENT(FOnEntityPicked, int32_t);

	class PickerRenderPass : public RenderPass
	{
	public:
		void Init() override;

		void Render(const FMeshRenderDatas &data);

		void Pick(const glm::uvec2 mousePos);

		void CreateFramebuffer() override;

		bool IsEnabled() const { return RenderPass::IsEnabled() && mEnabled; }

		Ref<Framebuffer> GetFramebuffer() const { return mFrambuffer; }

		FOnEntityPickedEvent OnEntityPicked;

	private:
		bool mEnabled{false};
		Ref<Shader> mShader;
		glm::uvec2 mMousePos;
	};
} // namespace BHive