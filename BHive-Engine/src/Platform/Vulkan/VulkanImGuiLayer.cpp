#include "gfx/RenderCommand.h"
#include "core/Application.h"
#include "VulkanWindowContext.h"
#include "VulkanRendererAPI.h"
#include "VulkanSwapChain.h"
#include "VulkanImGuiLayer.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <GLFW/glfw3.h>
#include "gfx/Texture.h"

namespace BHive
{
	struct VulkanViewportData
	{
		GLFWwindow *Window = nullptr;
		VulkanSwapChain *SwapChain = nullptr;

		vk::raii::CommandPool CmdPool = VK_NULL_HANDLE;
		std::vector<vk::raii::CommandBuffer> Cmds;
		uint32_t CurrentFrame = 0;
		uint32_t ImageIndex = 0;

		uint32_t Width = 0;
		uint32_t Height = 0;
		bool PendingResize = false;
		bool Minimized = false;
	};

	namespace callbacks
	{
		void CheckVkResult(VkResult result)
		{
			auto result_str = vk::to_string((vk::Result)result);
			ASSERT(result == VK_SUCCESS, fmt::runtime(result_str));
			return;
		}

		void ImCreateWindow(ImGuiViewport *vp)
		{
			GLFWwindow *window = (GLFWwindow *)vp->PlatformHandle;
		}

	} // namespace callbacks

	VulkanImGuiLayer::VulkanImGuiLayer(GLFWwindow *windowHandle)
		: mDevice(VulkanBackend::GetLogicalDevice()),
		  mWindowHandle(windowHandle)
	{
	}

	void VulkanImGuiLayer::BeginFrame()
	{
		ImGui_ImplVulkan_NewFrame();

		ImGuiLayer::BeginFrame();
	}

	void VulkanImGuiLayer::Init()
	{
		ImGuiLayer::Init();

		auto &instance = VulkanBackend::GetInstance();
		auto &physical_device = VulkanBackend::GetPhysicalDevice();
		auto &queue_familes = VulkanBackend::GetQueueFamilies();
		auto &device = VulkanBackend::GetLogicalDevice();
		auto &app = Application::Get();
		auto &window = app.GetWindow();

		auto &swap_chain = Cast<VulkanWindowContext>(window.GetContext())->GetSwapchain();
		auto extent = swap_chain.GetExtent();
		auto image_count = swap_chain.GetImageCount();

		std::vector<vk::DescriptorPoolSize> pool_sizes;
		pool_sizes.emplace_back(vk::DescriptorType::eCombinedImageSampler, 1000);
		pool_sizes.emplace_back(vk::DescriptorType::eSampledImage, 1000);
		pool_sizes.emplace_back(vk::DescriptorType::eSampler, 1000);

		vk::DescriptorPoolCreateInfo pool_create_info(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, 1000, pool_sizes);
		mDescriptorPool = vk::raii::DescriptorPool(device, pool_create_info);

		ImGui_ImplGlfw_InitForVulkan(mWindowHandle, true);

		auto format = swap_chain.GetFormat().format;
		auto depth_format = swap_chain.GetDepthStencilFormat();
		vk::PipelineRenderingCreateInfo rendering_info{};
		rendering_info.setViewMask(0).setColorAttachmentCount(1).setColorAttachmentFormats(format).setDepthAttachmentFormat(depth_format).setStencilAttachmentFormat(depth_format);

		ImGui_ImplVulkan_InitInfo init_info{};
		init_info.ApiVersion = MINIMUM_VULKAN_API_VERSION;
		init_info.Instance = *instance;
		init_info.PhysicalDevice = *physical_device;
		init_info.Device = *mDevice;
		init_info.Queue = *queue_familes.GraphicsQueue;
		init_info.QueueFamily = queue_familes.GraphicsQueueIndex;
		init_info.DescriptorPool = *mDescriptorPool;
		init_info.MinImageCount = swap_chain.GetMinImageCount();
		init_info.ImageCount = image_count;
		init_info.PipelineCache = VK_NULL_HANDLE;
		init_info.Allocator = VK_NULL_HANDLE;
		init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.PipelineInfoMain.RenderPass = VK_NULL_HANDLE;
		init_info.PipelineInfoMain.PipelineRenderingCreateInfo = rendering_info;
		init_info.PipelineInfoForViewports.PipelineRenderingCreateInfo = rendering_info;
		init_info.PipelineInfoForViewports.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.PipelineInfoMain.Subpass = 0;
		init_info.CheckVkResultFn = callbacks::CheckVkResult;
		init_info.UseDynamicRendering = true;

		ImGui_ImplVulkan_Init(&init_info);

		ImGuiPlatformIO &platform_io = ImGui::GetPlatformIO();
		// platform_io.Platform_CreateWindow = callbacks::ImCreateWindow;
	}

	void VulkanImGuiLayer::Shutdown()
	{
		LOG_TRACE("ImguiLayer Shutdown Called")

		mDevice.waitIdle();

		mDescriptorPool.reset();

		ImGui_ImplVulkan_Shutdown();

		ImGuiLayer::Shutdown();
	}

	void VulkanImGuiLayer::OnSubmitRenderData(ImDrawData *drawData)
	{
		auto &renderer = Renderer::Get();

		FPassState state{};
		auto &pass = renderer.BeginPass("ImGui", EPassType::Present, state);
		pass.BeginPhase();
		pass.Emplace<CmdImGuiRender>()(drawData);
		pass.EndPhase();

		renderer.EndPass();
	}

} // namespace BHive