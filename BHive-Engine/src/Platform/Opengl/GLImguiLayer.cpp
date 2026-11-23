#include "GLImguiLayer.h"
#include "core/Window.h"
#include "core/Application.h"
#include "core/WindowInput.h"
#include "gfx/RenderCommand.h"
#include "gfx/Texture.h"
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace BHive
{
	GLImGuiLayer::GLImGuiLayer(GLFWwindow *windowHandle)
		: mWindowHandle(windowHandle)
	{
	}

	void GLImGuiLayer::BeginFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();

		ImGuiLayer::BeginFrame();
	}

	void GLImGuiLayer::Init()
	{
		ImGuiLayer::Init();

		ImGui_ImplGlfw_InitForOpenGL(mWindowHandle, true);
		ImGui_ImplGlfw_SetCallbacksChainForAllWindows(true);

		ImGui_ImplOpenGL3_Init();
	}

	void GLImGuiLayer::Shutdown()
	{
		ImGui_ImplOpenGL3_Shutdown();

		ImGuiLayer::Shutdown();
	}

	void GLImGuiLayer::OnSubmitRenderData(ImDrawData *drawData)
	{
		auto &renderer = Renderer::Get();

		FPassState state{};
		for (const auto &priorPass : renderer.GetActiveGraph().GetPasses())
		{
			if (priorPass.Type == EPassType::Present)
			{
				state.Color.LoadOP = EAttachmentLoadState::Load;
				break;
			}
		}

		auto &pass = renderer.BeginPass("ImGui", EPassType::Present, state);
		pass.BeginPhase(EPhaseType::Graphics);
		pass.Emplace<CmdImGuiRender>()(drawData);
		pass.EndPhase();

		renderer.EndPass();
	}
} // namespace BHive