#pragma once

#include "GUICore.h"
#include "core/Layer.h"

struct GLFWwindow;

namespace BHive
{
	/*gui class that uses imgui*/
	struct BHIVE ImGuiLayer : public Layer
	{
		ImGuiLayer(GLFWwindow *window);
		~ImGuiLayer() = default;

		void OnAttach() override;
		void OnDetach() override;
		void OnEvent(Event &event) override;

		void BeginFrame();
		void EndFrame(uint32_t w, uint32_t h);

		void SetColorsDark();

		void BlockEvents(bool block);

		ImGuiContext *get_context() { return context; }

	private:
		void Init();
		void Shutdown();

		bool mBlockEvents{false};
		GLFWwindow *mWindow = nullptr;
		ImGuiContext *context = nullptr;
	};
}