#include "core/Application.h"
#include "core/events/Event.h"
#include "core/Window.h"
#include "gfx/RenderCommand.h"
#include "ImGuiLayer.h"
#include "Platform/Vulkan/VulkanGraphicsContext.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"
#include "Platform/Vulkan/VulkanSwapChain.h"
#include <backends/imgui_impl_glfw.cpp>
#include <backends/imgui_impl_glfw.h>
// #include <backends/imgui_impl_opengl3.cpp>
// #include <backends/imgui_impl_opengl3.h>

#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>

// #define IMGUI_IMPL_VULKAN_USE_VOLK
// #define VOLK_IMPLEMENTATION
// #include <vol>

#include "Platform/Vulkan/VulkanUtils.h"
#include <backends/imgui_impl_vulkan.cpp>
#include <backends/imgui_impl_vulkan.h>

#include <imgui.h>
#include <implot.h>

#ifdef IMGUI_SHARED
	#undef IMGUI_SHARED
	#define IMGUI_API
#endif

#include <ImGuizmo.h>
#include "gfx/Texture.h"

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

		auto &context = static_cast<VulkanGraphicsContext &>(GraphicsContext::Get());
		auto &instance = VulkanCore::GetInstance();
		auto &physical_device = VulkanCore::GetPhysicalDevice();
		auto &device = VulkanCore::GetLogicalDevice();
		auto &swap_chain = context.GetSwapChain();
		auto extent = swap_chain->GetExtent();
		auto image_count = swap_chain->GetImageCount();
		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();
		auto pool_size = IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE;

		// mDescriptorSetLayout = FDescriptorSetLayout::Builder().AddBinding(0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment, 1).Build();
		mDescriptorPool = FDescriptorPool::Builder()
							  .SetMaxSets(8)
							  .AddPoolSize(vk::DescriptorType::eSampler, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE)
							  .AddPoolSize(vk::DescriptorType::eCombinedImageSampler, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE)
							  .AddPoolSize(vk::DescriptorType::eSampledImage, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE)
							  .AddPoolSize(vk::DescriptorType::eStorageImage, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE)
							  .AddPoolSize(vk::DescriptorType::eUniformTexelBuffer, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE)
							  .AddPoolSize(vk::DescriptorType::eStorageTexelBuffer, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE)
							  .AddPoolSize(vk::DescriptorType::eUniformBuffer, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE)
							  .AddPoolSize(vk::DescriptorType::eStorageBuffer, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE)
							  .AddPoolSize(vk::DescriptorType::eUniformBufferDynamic, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE)
							  .AddPoolSize(vk::DescriptorType::eStorageBufferDynamic, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE)
							  .AddPoolSize(vk::DescriptorType::eInputAttachment, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE)
							  .Build();

		// FDescriptorWriter(mDescriptorSetLayout, mDescriptorPool).Build(mDescriptorSets);

		ImGui_ImplGlfw_InitForVulkan(mWindow, true);

		mCommandBuffers = api->AllocateCommandBuffers(image_count);

		VulkanCore::RegisterOnDeviceCreated(
			[this, &device, image_count]()
			{
				auto &device = VulkanCore::GetLogicalDevice();
				auto api = RenderCommand::GetAPI<VulkanRendererAPI>();
				mCommandBuffers = api->AllocateCommandBuffers(image_count);
			});

		VulkanCore::RegisterOnDeviceDestroy([this]() { mCommandBuffers = nullptr; });

		auto format = swap_chain->GetFormat().format;
		auto depth_format = VulkanUtils::FindDepthFormat(physical_device);
		vk::PipelineRenderingCreateInfo rendering_info(0, format, depth_format, vk::Format::eUndefined);

		auto &queue_familes = VulkanCore::GetQueueFamilies();

		ImGui_ImplVulkan_InitInfo init_info{};
		init_info.ApiVersion = VulkanCore::MINIMUM_VULKAN_API_VERSION;
		init_info.Instance = *instance;
		init_info.PhysicalDevice = *physical_device;
		init_info.Device = *device;
		init_info.Queue = *queue_familes.GraphicsQueue;
		init_info.QueueFamily = queue_familes.GraphicsQueueIndex;
		init_info.DescriptorPool = *mDescriptorPool->GetPool();
		init_info.MinImageCount = swap_chain->GetMinImageCount();
		init_info.ImageCount = image_count;
		init_info.PipelineCache = VK_NULL_HANDLE;
		init_info.Allocator = VK_NULL_HANDLE;
		init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.PipelineInfoMain.RenderPass = VK_NULL_HANDLE;
		init_info.PipelineInfoMain.PipelineRenderingCreateInfo = rendering_info;

		init_info.PipelineInfoForViewports.PipelineRenderingCreateInfo = rendering_info;
		init_info.PipelineInfoForViewports.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.PipelineInfoMain.Subpass = 0;
		init_info.CheckVkResultFn = CheckVkResult;
		init_info.UseDynamicRendering = true;

		ImGui_ImplVulkan_Init(&init_info);
	}

	void ImGuiLayer::Shutdown()
	{
		auto &device = VulkanCore::GetLogicalDevice();
		device.waitIdle();

		ClearTextureMap();

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
		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();

		auto imgui_command = [=](const FVulkanFrameData &data)
		{
			auto &cmd = mCommandBuffers->at(data.Frame);

			vk::ClearValue clear_color = vk::ClearColorValue({0, 0, 0, 1});
			vk::RenderingAttachmentInfoKHR color_attachment(
				data.ImageView, vk::ImageLayout::eColorAttachmentOptimal, vk::ResolveModeFlagBits::eNone, {}, vk::ImageLayout::eUndefined, vk::AttachmentLoadOp::eLoad, vk::AttachmentStoreOp::eStore,
				clear_color);

			vk::RenderingInfo render_info({}, {{0, 0}, {(uint32_t)size.x, (uint32_t)size.y}}, 1, 0, color_attachment);

			RecordImGuiDrawCommands(ImGui::GetDrawData(), cmd, render_info);
		};

		api->SubmitCommand(imgui_command);

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow *backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			if (backup_current_context != glfwGetCurrentContext())
				glfwMakeContextCurrent(backup_current_context);
		}
	}

	void ImGuiLayer::RecordImGuiDrawCommands(ImDrawData *drawData, vk::raii::CommandBuffer &cmd, const vk::RenderingInfo &renderingInfo)
	{
		cmd.reset();
		cmd.begin({});
		cmd.beginRendering(renderingInfo);

		ImGui_ImplVulkan_RenderDrawData(drawData, *cmd);

		cmd.endRendering();

		cmd.end();
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

	VkDescriptorSet ImGuiLayer::GetTextureID(const Ref<Texture> &texture)
	{
		if (s_ImGuiTextureMap.contains(texture))
			return s_ImGuiTextureMap[texture];

		auto handle = reinterpret_cast<const vk::DescriptorImageInfo *>(texture->GetNativeHandle());
		auto set = ImGui_ImplVulkan_AddTexture(handle->sampler, handle->imageView, (VkImageLayout)handle->imageLayout);
		return s_ImGuiTextureMap[texture] = set;
	}

	void ImGuiLayer::ClearTextureMap()
	{
		for (auto [_, set] : s_ImGuiTextureMap)
		{
			ImGui_ImplVulkan_RemoveTexture(set);
		}
		s_ImGuiTextureMap.clear();
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