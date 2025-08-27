#pragma once

#include "core/Core.h"
#include "renderers/RenderData.h"
#include "core/EventDelegate.h"

namespace BHive
{
	class Framebuffer;
	class Shader;

	DECLARE_EVENT(FOnEntityPicked, int32_t);

	class PickerRenderPass
	{
	public:
		void Init();

		void CreateResizableObjects(const glm::uvec2 &size);

		void Resize(const glm::uvec2 &size);

		void Begin();

		void End();

		void Render(const FMeshRenderDatas &data);

		void Pick(const glm::uvec2 mousePos);

		void CreateFramebuffer();

		bool IsEnabled() const { return mEnabled; }

		Ref<Framebuffer> GetFramebuffer() const { return mFrambuffer; }

		FOnEntityPickedEvent OnEntityPicked;

	private:
		bool mEnabled{false};

		Ref<Framebuffer> mFrambuffer;
		Ref<Shader> mShader;

		glm::uvec2 mSize;
		glm::uvec2 mMousePos;
	};
} // namespace BHive