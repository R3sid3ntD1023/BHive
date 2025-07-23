#pragma once

#include "core/Layer.h"

struct GLFWwindow;
struct ImGuiContext;

namespace BHive
{
	/*gui class that uses imgui*/
	struct BHIVE_API ImGuiLayer : public Layer
	{
		ImGuiLayer(GLFWwindow *window);
		~ImGuiLayer() = default;

		void OnAttach() override;
		void OnDetach() override;
		void OnEvent(Event &event) override;

		void BeginFrame();
		void EndFrame();

		void SetColorsDark();

		void BlockEvents(bool block);

	private:
		void Init();
		void Shutdown();

		bool mBlockEvents{false};
		GLFWwindow *mWindow = nullptr;
	};
} // namespace BHive