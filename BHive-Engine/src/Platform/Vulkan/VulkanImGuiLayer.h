#pragma once

#include "core/layers/ImGuiLayer.h"
#include "VulkanBackend.h"

namespace BHive
{
	class FDescriptorPool;

	class BHIVE_API VulkanImGuiLayer : public ImGuiLayer
	{
	public:
		VulkanImGuiLayer(GLFWwindow *windowHandle);

		~VulkanImGuiLayer();

		void BeginFrame() override;

		void ClearTextureMap();

	protected:
		void Init() override;

		void Shutdown() override;

		void OnSubmitRenderData(ImDrawData *drawData, const glm::ivec2 &pos, const glm::uvec2 &size) override;

		void OnInvalidateTexture(const Texture &tex) override;

		ImTextureRef GetTextureIDImpl(const Texture &texture) override;

	private:
		vk::raii::Device &mDevice;

		GLFWwindow *mWindowHandle = nullptr;

		vk::raii::DescriptorPool mDescriptorPool = VK_NULL_HANDLE;

		std::unordered_map<EngineResourceID, VkDescriptorSet> mImGuiTextureMap;
	};

} // namespace BHive