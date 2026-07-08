#pragma once

#include "core/layers/ImGuiLayer.h"
#include "VulkanBackend.h"

namespace BHive
{
	class BHIVE_API VulkanImGuiLayer : public ImGuiLayer
	{
	public:
		VulkanImGuiLayer(GLFWwindow *windowHandle);

		void BeginFrame() override;

	protected:
		void Init() override;

		void Shutdown() override;

		void OnSubmitRenderData(ImDrawData *drawData, const glm::ivec2 &pos, const glm::uvec2 &size) override;

	private:
		vk::raii::Device &mDevice;

		GLFWwindow *mWindowHandle = nullptr;

		vk::raii::DescriptorPool mDescriptorPool = VK_NULL_HANDLE;
	};

} // namespace BHive