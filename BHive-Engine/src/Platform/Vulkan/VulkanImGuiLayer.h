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

		void OnSubmitRenderData(ImDrawData *drawData) override;

	private:
		GLFWwindow *mWindowHandle = nullptr;

		vk::raii::DescriptorPool mDescriptorPool = VK_NULL_HANDLE;
	};

} // namespace BHive