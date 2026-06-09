#include "core/Application.h"
#include "core/Window.h"
#include "ImGuiLayer.h"
#include "core/Time.h"

#include "Platform/Vulkan/VulkanImGuiLayer.h"
#include <backends/imgui_impl_glfw.h>

#include <glfw/glfw3.h>
#include "gfx/RenderCommand.h"

#include <imgui.h>
#include <ImGuizmo.h>
#include <implot.h>

namespace BHive
{
	void ImGuiLayer::OnAttach(Application& app)
	{
		Init();
	}

	void ImGuiLayer::OnDetach()
	{
		Shutdown();
	}

	void ImGuiLayer::OnEvent(Event &event)
	{
		if (mBlockEvents)
		{
			auto &io = ImGui::GetIO();

			event.mHandled |= event.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			event.mHandled |= event.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}

	void ImGuiLayer::Init()
	{
		IMGUI_CHECKVERSION();
		auto ctx = ImGui::CreateContext();
		auto implot_ctx = ImPlot::CreateContext();

		ImGui::SetCurrentContext(ctx);
		ImPlot::SetCurrentContext(implot_ctx);

		ImGuiIO &io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
		// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // IF using Docking Branch
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGuiStyle &style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		float font_size = 14.f;
		io.Fonts->AddFontFromFileTTF(ENGINE_PATH "/data/fonts/Roboto/Roboto-Bold.ttf", font_size);
		io.FontDefault = io.Fonts->AddFontFromFileTTF(ENGINE_PATH "/data/fonts/Roboto/Roboto-Regular.ttf", font_size);

		SetColorsDark();
	}

	void ImGuiLayer::Shutdown()
	{
		ImGui_ImplGlfw_Shutdown();
		ImPlot::DestroyContext();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::BeginFrame()
	{

		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiLayer::EndFrame()
	{
		auto &window = Application::Get().GetWindow();
		const auto& pos = window.GetPosition();
		const auto& size = window.GetSize();

		ImGuiIO &io = ImGui::GetIO();
		io.DisplaySize = {(float)size.x, (float)size.y};
		io.DeltaTime = Time::DeltaTime();

		ImGui::Render();

		OnRender(ImGui::GetDrawData(), pos, size);

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow *backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			if (backup_current_context != glfwGetCurrentContext())
				glfwMakeContextCurrent(backup_current_context);
		}
	}

	void ImGuiLayer::SetColorsDark()
	{
		auto &colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{0.1f, 0.105f, 0.11f, 1.0f};
		colors[ImGuiCol_ChildBg] = ImVec4{0.15f, 0.155, .15f, 1.0f};

		// Headers
		colors[ImGuiCol_Header] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
		colors[ImGuiCol_HeaderHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
		colors[ImGuiCol_HeaderActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

		// Buttons
		colors[ImGuiCol_Button] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
		colors[ImGuiCol_ButtonHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
		colors[ImGuiCol_ButtonActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
		colors[ImGuiCol_FrameBgHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
		colors[ImGuiCol_FrameBgActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
		colors[ImGuiCol_TabHovered] = ImVec4{0.38f, 0.3805f, 0.381f, 1.0f};
		colors[ImGuiCol_TabActive] = ImVec4{0.28f, 0.2805f, 0.281f, 1.0f};
		colors[ImGuiCol_TabUnfocused] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};

		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
		colors[ImGuiCol_TitleBgActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
	}

	ImTextureRef ImGuiLayer::GetTextureID(const Texture &texture)
	{
		auto &app = Application::Get();
		auto layer = app.GetImGuiLayer();
		return layer->GetTextureIDImpl(texture);
	}

	void ImGuiLayer::BlockEvents(bool block)
	{
		mBlockEvents = block;
	}

	void *ImGuiLayer::GetContext() const
	{
		return ImGui::GetCurrentContext();
	}

	void ImGuiLayer::GetAllocatorCallbacks(void *alloc_func, void *free_func, void **user_data) const
	{
		ImGui::GetAllocatorFunctions((ImGuiMemAllocFunc *)alloc_func, (ImGuiMemFreeFunc *)free_func, user_data);
	}

	Ref<ImGuiLayer> ImGuiLayer::Create(GLFWwindow *window)
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::EAPI::Vulkan:
			return CreateRef<VulkanImGuiLayer>(window);
		default:
			break;
		}

		ASSERT(false);
		return nullptr;
	}
} // namespace BHive