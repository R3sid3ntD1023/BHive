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

		void *GetContext() const;
		void GetAllocatorCallbacks(void *alloc_func, void *free_func, void **user_data) const;

	private:
		void Init();
		void Shutdown();

		bool mBlockEvents{false};
		GLFWwindow *mWindow = nullptr;
	};
} // namespace BHive