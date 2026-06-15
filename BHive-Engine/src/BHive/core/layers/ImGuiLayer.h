#pragma once

#include "core/Layer.h"
#include "gui/GUICore.h"

struct GLFWwindow;
struct ImDrawData;

namespace BHive
{
	class Window;
	class Texture;

	/*gui class that uses imgui*/
	class BHIVE_API ImGuiLayer : public Layer
	{
	public:
		virtual ~ImGuiLayer() = default;

		void OnAttach(Application&) override;

		void OnDetach() override;

		void OnEvent(Event &event) override;

		virtual void BeginFrame();

		virtual void EndFrame();

		void SetColorsDark();

		void BlockEvents(bool block);

		void *GetContext() const;

		void GetAllocatorCallbacks(void *alloc_func, void *free_func, void **user_data) const;

		static ImTextureRef GetTextureID(const Texture &texture);

		static Ref<ImGuiLayer> Create(GLFWwindow *window);

	protected:
		virtual void Init();

		virtual void Shutdown();

		virtual void OnSubmitRenderData(ImDrawData *drawData, const glm::ivec2 &pos, const glm::uvec2 &size) {};

		virtual ImTextureRef GetTextureIDImpl(const Texture &texture) = 0;

	private:
		bool mBlockEvents{false};
	};
} // namespace BHive