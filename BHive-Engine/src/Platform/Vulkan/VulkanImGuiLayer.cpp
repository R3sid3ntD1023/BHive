#include "gfx/RenderCommand.h"
#include "Platform/Vulkan/DescriptorBuilder.h"
#include "Platform/Vulkan/VulkanGraphicsContext.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"
#include "Platform/Vulkan/VulkanSwapChain.h"
#include "Platform/Vulkan/VulkanUtils.h"
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
		: mDevice(VulkanCore::GetLogicalDevice()),
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

		auto &context = static_cast<VulkanGraphicsContext &>(GraphicsContext::Get());
		auto &instance = VulkanCore::GetInstance();
		auto &physical_device = VulkanCore::GetPhysicalDevice();
		auto &swap_chain = context.GetSwapChain();
		auto extent = swap_chain->GetExtent();
		auto image_count = swap_chain->GetImageCount();
		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();
		auto &queue_familes = VulkanCore::GetQueueFamilies();

		mDescriptorPool = FDescriptorPool::Builder().SetMaxSets(2).AddPoolSize(vk::DescriptorType::eCombinedImageSampler, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE).Build();

		ImGui_ImplGlfw_InitForVulkan(mWindowHandle, true);

		/*vk::CommandPoolCreateInfo pool_info(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, queue_familes.GraphicsQueueIndex);
		mCommandPool = mDevice.createCommandPool(pool_info);*/

		// vk::CommandBufferAllocateInfo alloc_info(mCommandPool, vk::CommandBufferLevel::ePrimary, image_count);
		// mCommandBuffers = vk::raii::CommandBuffers(mDevice, alloc_info);

		mCommandBuffers = api->AllocateCommandBuffers(image_count);

		auto format = swap_chain->GetFormat().format;
		auto depth_format = VulkanUtils::FindDepthFormat(physical_device);
		vk::PipelineRenderingCreateInfo rendering_info(0, format, depth_format, vk::Format::eUndefined);

		ImGui_ImplVulkan_InitInfo init_info{};
		init_info.ApiVersion = VulkanCore::MINIMUM_VULKAN_API_VERSION;
		init_info.Instance = *instance;
		init_info.PhysicalDevice = *physical_device;
		init_info.Device = *mDevice;
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
		init_info.CheckVkResultFn = callbacks::CheckVkResult;
		init_info.UseDynamicRendering = true;

		ImGui_ImplVulkan_Init(&init_info);
	}

	void VulkanImGuiLayer::Shutdown()
	{
		mDevice.waitIdle();

		ClearTextureMap();

		ImGui_ImplVulkan_Shutdown();

		ImGuiLayer::Shutdown();
	}

	void VulkanImGuiLayer::OnRender(ImDrawData *drawData, const glm::uvec2 &displaySize)
	{
		auto api = RenderCommand::GetAPI<VulkanRendererAPI>();

		auto imgui_command = [=](const FVulkanFrameData &data)
		{
			auto &cmd = mCommandBuffers->at(data.Frame);

			vk::ClearValue clear_color = vk::ClearColorValue({0, 0, 0, 1});
			vk::RenderingAttachmentInfoKHR color_attachment(
				data.ImageView, vk::ImageLayout::eColorAttachmentOptimal, vk::ResolveModeFlagBits::eNone, {}, vk::ImageLayout::eUndefined, vk::AttachmentLoadOp::eLoad, vk::AttachmentStoreOp::eStore,
				clear_color);

			vk::RenderingInfo render_info({}, {{0, 0}, {displaySize.x, displaySize.y}}, 1, 0, color_attachment);
			vk::CommandBufferBeginInfo begin_info{};

			cmd.reset();
			cmd.begin(begin_info);

			cmd.beginRendering(render_info);

			ImGui_ImplVulkan_RenderDrawData(drawData, *cmd);

			cmd.endRendering();

			cmd.end();
		};

		api->SubmitSecondaryCommand(imgui_command);
	}

	ImTextureRef VulkanImGuiLayer::GetTextureIDImpl(const Texture &texture)
	{

		auto handle = reinterpret_cast<const vk::DescriptorImageInfo *>(texture.GetNativeHandle());

		if (s_ImGuiTextureMap.contains(handle))
			return s_ImGuiTextureMap[handle];

		auto set = ImGui_ImplVulkan_AddTexture(handle->sampler, handle->imageView, (VkImageLayout)handle->imageLayout);
		return s_ImGuiTextureMap[handle] = set;
	}

} // namespace BHive