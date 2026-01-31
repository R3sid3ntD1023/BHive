#pragma once

#include "core/layers/ImGuiLayer.h"
#include "Platform/Vulkan/VulkanCore.h"

namespace BHive
{
	class FDescriptorPool;

	class BHIVE_API VulkanImGuiLayer : public ImGuiLayer
	{
	public:
		VulkanImGuiLayer(GLFWwindow *windowHandle);

		virtual void BeginFrame() override;

		static void ClearTextureMap();

	protected:
		virtual void Init() override;

		virtual void Shutdown() override;

		virtual void OnRender(ImDrawData *drawData, const glm::uvec2 &displaySize) override;

		virtual ImTextureRef GetTextureIDImpl(const Texture &texture) override;

	private:
		vk::raii::Device &mDevice;

		GLFWwindow *mWindowHandle = nullptr;

		Ref<FDescriptorPool> mDescriptorPool;

		static inline std::unordered_map<const vk::DescriptorImageInfo *, VkDescriptorSet> s_ImGuiTextureMap;
	};

} // namespace BHive