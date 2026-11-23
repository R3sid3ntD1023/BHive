#pragma once

#include "core/layers/ImGuiLayer.h"

namespace BHive
{
	class GLImGuiLayer : public ImGuiLayer
	{
	public:
		GLImGuiLayer(GLFWwindow *windowHandle);

		void BeginFrame() override;

	protected:
		void Init() override;

		void Shutdown() override;

		void OnSubmitRenderData(ImDrawData *drawData) override;

	private:
		GLFWwindow *mWindowHandle = nullptr;
	};

} // namespace BHive