#pragma once

#include "core/layers/ImGuiLayer.h"
#include "VulkanBackend.h"

namespace BHive
{
	class FDescriptorPool;

	struct TextureKey
	{
		VkImageView View;
		VkSampler Sampler;

		bool operator==(const TextureKey& rhs) const { return View == rhs.View && Sampler == rhs.Sampler;}
	};

	struct TextureKeyHash
	{
		size_t operator()(const TextureKey &k) const { return std::hash<VkImageView>()(k.View) ^ (std::hash<VkSampler>()(k.Sampler) << 1); }
	};

	class BHIVE_API VulkanImGuiLayer : public ImGuiLayer
	{
	public:
		VulkanImGuiLayer(GLFWwindow *windowHandle);

		void BeginFrame() override;

		static void ClearTextureMap();

	protected:
		void Init() override;

		void Shutdown() override;

		void OnSubmitRenderData(ImDrawData *drawData, const glm::ivec2 &pos, const glm::uvec2 &size) override;

		ImTextureRef GetTextureIDImpl(const Texture &texture) override;

	private:
		vk::raii::Device &mDevice;

		GLFWwindow *mWindowHandle = nullptr;

		vk::raii::DescriptorPool mDescriptorPool = VK_NULL_HANDLE;

		static inline std::unordered_map<TextureKey, VkDescriptorSet, TextureKeyHash> s_ImGuiTextureMap;
	};

} // namespace BHive