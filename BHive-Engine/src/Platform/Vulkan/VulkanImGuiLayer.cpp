#include "gfx/RenderCommand.h"
#include "VulkanWindowContext.h"
#include "VulkanRendererAPI.h"
#include "VulkanSwapChain.h"
#include "VulkanUtils.h"
#include "VulkanImGuiLayer.h"

#include <backends/imgui_impl_glfw.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <backends/imgui_impl_vulkan.h>
#include "gfx/Texture.h"

namespace BHive
{
	namespace callbacks
	{
		void CheckVkResult(VkResult result)
		{
			auto result_str = vk::to_string((vk::Result)result);
			ASSERT(result == VK_SUCCESS, result_str);
			return;
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

	void VulkanImGuiLayer::ClearTextureMap()
	{
		for (auto [_, set] : s_ImGuiTextureMap)
		{
			ImGui_ImplVulkan_RemoveTexture(set);
		}
		s_ImGuiTextureMap.clear();
	}

	void VulkanImGuiLayer::Init()
	{
		ImGuiLayer::Init();

		auto &context = static_cast<VulkanWindowContext &>(WindowContext::Get());
		auto &instance = VulkanBackend::GetInstance();
		auto &physical_device = VulkanBackend::GetPhysicalDevice();
		auto &swap_chain = context.GetSwapChain();
		auto extent = swap_chain->GetExtent();
		auto image_count = swap_chain->GetImageCount();
		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();
		auto &queue_familes = VulkanBackend::GetQueueFamilies();
		auto &device = VulkanBackend::GetLogicalDevice();

		std::vector<vk::DescriptorPoolSize> pool_sizes;
		pool_sizes.emplace_back(vk::DescriptorType::eSampler, 1000);
		pool_sizes.emplace_back(vk::DescriptorType::eCombinedImageSampler, 1000);
		pool_sizes.emplace_back(vk::DescriptorType::eSampledImage, 1000);
		pool_sizes.emplace_back(vk::DescriptorType::eStorageImage, 1000);
		pool_sizes.emplace_back(vk::DescriptorType::eUniformTexelBuffer, 1000);
		pool_sizes.emplace_back(vk::DescriptorType::eStorageTexelBuffer, 1000);
		pool_sizes.emplace_back(vk::DescriptorType::eUniformBuffer, 1000);
		pool_sizes.emplace_back(vk::DescriptorType::eStorageBuffer, 1000);
		pool_sizes.emplace_back(vk::DescriptorType::eUniformBufferDynamic, 1000);
		pool_sizes.emplace_back(vk::DescriptorType::eStorageBufferDynamic, 1000);
		pool_sizes.emplace_back(vk::DescriptorType::eInputAttachment, 1000);
		
		vk::DescriptorPoolCreateInfo pool_create_info({}, 1000, pool_sizes);
		mDescriptorPool = vk::raii::DescriptorPool(device, pool_create_info);
								

		ImGui_ImplGlfw_InitForVulkan(mWindowHandle, true);

		auto format = swap_chain->GetFormat().format;
		auto depth_format = swap_chain->GetDepthStencilFormat();
		vk::PipelineRenderingCreateInfo rendering_info{};
		rendering_info.setViewMask(0).setColorAttachmentCount(1).setColorAttachmentFormats(format).setDepthAttachmentFormat(depth_format).setStencilAttachmentFormat(depth_format);

		ImGui_ImplVulkan_InitInfo init_info{};
		init_info.ApiVersion = VulkanBackend::MINIMUM_VULKAN_API_VERSION;
		init_info.Instance = *instance;
		init_info.PhysicalDevice = *physical_device;
		init_info.Device = *mDevice;
		init_info.Queue = *queue_familes.GraphicsQueue;
		init_info.QueueFamily = queue_familes.GraphicsQueueIndex;
		init_info.DescriptorPool = *mDescriptorPool;
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
		init_info.CheckVkResultFn = callbacks::CheckVkResult;
		init_info.UseDynamicRendering = true;

		ImGui_ImplVulkan_Init(&init_info);
	}

	void VulkanImGuiLayer::Shutdown()
	{
		LOG_TRACE("ImguiLayer Shutdown Called")

		mDevice.waitIdle();

		ClearTextureMap();

		mDescriptorPool.reset();

		ImGui_ImplVulkan_Shutdown();

		ImGuiLayer::Shutdown();

		
	}

	void VulkanImGuiLayer::OnRender(ImDrawData *drawData, const glm::uvec2 &displaySize)
	{
		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();

		auto imgui_command = [=](const FVulkanFrame &data)
		{
			auto& cmd = data.CommandBuffer;

			//LOG_INFO("CMD: Imgui Render, frame={}", data.Frame);

			vk::Viewport viewport(0.0f, 0.0f, (float)displaySize.x, (float)displaySize.y, 0.0f, 1.0f);
			vk::Rect2D scissor({0, 0}, {(uint32_t)displaySize.x, (uint32_t)displaySize.y});
		
			cmd.setViewport(0, viewport);
			cmd.setScissor(0, scissor);

	
			ImGui_ImplVulkan_RenderDrawData(drawData, *cmd);
		};

		api->SubmitCommand(imgui_command);
	}

	ImTextureRef VulkanImGuiLayer::GetTextureIDImpl(const Texture &texture)
	{
		auto handle = texture.GetNativeHandle().As<vk::DescriptorImageInfo>();

		if (s_ImGuiTextureMap.contains(handle))
			return s_ImGuiTextureMap[handle];

		auto set = ImGui_ImplVulkan_AddTexture(handle->sampler, handle->imageView, (VkImageLayout)handle->imageLayout);
		return s_ImGuiTextureMap[handle] = set;
	}

} // namespace BHive