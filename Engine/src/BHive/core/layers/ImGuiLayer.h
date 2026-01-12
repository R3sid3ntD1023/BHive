#pragma once

#include "core/Layer.h"
#include "gfx/DescriptorBuilder.h"
#include "gfx/VulkanCore.h"

struct GLFWwindow;
struct ImGuiContext;
struct ImDrawData;

namespace BHive
{
	class Texture;

	/*gui class that uses imgui*/
	struct BHIVE_API ImGuiLayer : public Layer
	{
		ImGuiLayer(GLFWwindow *window);
		~ImGuiLayer() = default;

		void OnAttach() override;
		void OnDetach() override;
		void OnEvent(Event &event) override;

		void BeginFrame();
		void EndFrame();

		void SetColorsDark();

		void BlockEvents(bool block);

		void *GetContext() const;

		void GetAllocatorCallbacks(void *alloc_func, void *free_func, void **user_data) const;

		const Ref<FDescriptorPool> &GetDescriptorPool() const { return mDescriptorPool; }

		static VkDescriptorSet GetTextureID(const Ref<Texture> &texture);

		static void ClearTextureMap();

	private:
		void Init();

		void Shutdown();

		void RecordImGuiDrawCommands(ImDrawData *drawData, vk::raii::CommandBuffer &cmd, const vk::RenderingInfo &renderingInfo);

		bool mBlockEvents{false};

		GLFWwindow *mWindow = nullptr;

		Ref<FDescriptorPool> mDescriptorPool;

		vk::raii::CommandBuffers *mCommandBuffers = nullptr;

		static inline std::unordered_map<Ref<Texture>, VkDescriptorSet> s_ImGuiTextureMap;
	};
} // namespace BHive