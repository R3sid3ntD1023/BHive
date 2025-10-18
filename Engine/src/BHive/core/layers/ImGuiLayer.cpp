#include "core/Application.h"
#include "core/events/Event.h"
#include "core/Window.h"
#include "gfx/GraphicsContext.h"
#include "gfx/RenderCommand.h"
#include "gfx/VulkanSwapChain.h"
#include "ImGuiLayer.h"
#include <backends/imgui_impl_glfw.cpp>
#include <backends/imgui_impl_glfw.h>
// #include <backends/imgui_impl_opengl3.cpp>
// #include <backends/imgui_impl_opengl3.h>

#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>

// #define IMGUI_IMPL_VULKAN_USE_VOLK
// #define VOLK_IMPLEMENTATION
// #include <vol>

#include "gfx/VulkanUtils.h"
#include <backends/imgui_impl_vulkan.cpp>
#include <backends/imgui_impl_vulkan.h>

#include <imgui.h>
#include <implot.h>

#ifdef IMGUI_SHARED
	#undef IMGUI_SHARED
	#define IMGUI_API
#endif

#include <ImGuizmo.h>

namespace BHive
{
	static void CheckVkResult(VkResult result)
	{
		auto result_str = vk::to_string((vk::Result)result);
		ASSERT(result == VK_SUCCESS, result_str);
		return;
	}

	ImGuiLayer::ImGuiLayer(GLFWwindow *window)
		: mWindow(window)
	{
	}

	void ImGuiLayer::OnAttach()
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
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

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

		auto &context = GraphicsContext::Get();
		auto &device = context.GetDevice();
		auto &swap_chain = context.GetSwapChain();
		auto extent = swap_chain->GetExtent();
		auto image_count = swap_chain->GetImageCount();
		auto api = RenderCommand::GetAPI();
		auto &command_pool = api->GetCommandPool();

		// create descriptor pool
		vk::DescriptorPoolSize pool_sizes[] = {{vk::DescriptorType::eCombinedImageSampler, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE}};
		vk::DescriptorPoolCreateInfo pool_info(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, 0, pool_sizes);
		for (auto &pool_size : pool_sizes)
			pool_info.maxSets += pool_size.descriptorCount;

		mDescriptorPool = device.createDescriptorPool(pool_info);

		ImGui_ImplGlfw_InitForVulkan(mWindow, true);

		mCommandBuffers = api->AllocateCommandBuffers(image_count);

		auto format = swap_chain->GetFormat().format;
		vk::PipelineRenderingCreateInfo rendering_info(0, format);

		ImGui_ImplVulkan_InitInfo init_info{};
		init_info.ApiVersion = VK_API_VERSION_1_4;
		init_info.Instance = *VulkanCore::GetInstance();
		init_info.PhysicalDevice = *VulkanCore::GetPhysicalDevice();
		init_info.Device = *device;
		init_info.Queue = *context.GetQueueFamilies().GraphicsQueue;
		init_info.QueueFamily = context.GetQueueFamilies().GraphicsQueueIndex;
		init_info.DescriptorPool = *mDescriptorPool;
		init_info.MinImageCount = swap_chain->GetMinImageCount();
		init_info.ImageCount = image_count;
		init_info.PipelineCache = VK_NULL_HANDLE;
		init_info.Allocator = VK_NULL_HANDLE;
		init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.PipelineInfoMain.RenderPass = VK_NULL_HANDLE;
		init_info.PipelineInfoMain.Subpass = 0;
		init_info.CheckVkResultFn = CheckVkResult;
		init_info.UseDynamicRendering = true;
		init_info.PipelineInfoMain.PipelineRenderingCreateInfo = rendering_info;

		ImGui_ImplVulkan_Init(&init_info);
	}

	void ImGuiLayer::Shutdown()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();

		ImPlot::DestroyContext();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::BeginFrame()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiLayer::EndFrame()
	{
		auto &window = Application::Get().GetWindow();
		auto size = window.GetSize();

		ImGuiIO &io = ImGui::GetIO();
		io.DisplaySize = {(float)size.x, (float)size.y};

		ImGui::Render();
		auto api = RenderCommand::GetAPI();

		auto imgui_command = [=](const FRenderCommand::FCommandData &data)
		{
			auto &context = GraphicsContext::Get();
			auto &swap_chain = context.GetSwapChain();
			auto current_frame = data.Frame;
			auto image_index = data.ImageIndex;
			auto &cmd = mCommandBuffers->at(current_frame);
			auto &image_view = swap_chain->GetImageView(image_index);
			auto &image = swap_chain->GetImage(image_index);

			vk::ClearValue clear_color = vk::ClearColorValue({0, 0, 0, 1});
			vk::RenderingAttachmentInfoKHR color_attachment(
				image_view, vk::ImageLayout::eColorAttachmentOptimal, vk::ResolveModeFlagBits::eNone, {}, vk::ImageLayout::eUndefined, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
				clear_color);

			vk::RenderingInfo render_info({}, {{0, 0}, {(uint32_t)size.x, (uint32_t)size.y}}, 1, 0, color_attachment);
			cmd.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
			cmd.beginRendering(render_info);

			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *cmd);

			cmd.endRendering();

			/*VulkanUtils::TransitionImageLayout(
				cmd, image, image_index, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR, vk::AccessFlagBits2::eColorAttachmentWrite, {},
				vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::PipelineStageFlagBits2::eBottomOfPipe);*/

			cmd.end();
		};

		api->SubmitSecondaryCommand(imgui_command);

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
} // namespace BHive