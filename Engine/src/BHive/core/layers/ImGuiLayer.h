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

		const Ref<FDescriptorSetLayout> &GetDescriptorSetLayout() const { return mDescriptorSetLayout; }

		vk::raii::DescriptorSets &GetDescriptorSets() { return mDescriptorSets; }

		vk::raii::DescriptorSet &GetDescriptorSet(uint32_t index) { return mDescriptorSets[index]; }

	private:
		void Init();

		void Shutdown();

		void RecordImGuiDrawCommands(ImDrawData* drawData, vk::raii::CommandBuffer& cmd, const vk::RenderingInfo& renderingInfo);

		bool mBlockEvents{false};

		GLFWwindow *mWindow = nullptr;

		Ref<FDescriptorSetLayout> mDescriptorSetLayout;

		Ref<FDescriptorPool> mDescriptorPool;

		vk::raii::DescriptorSets mDescriptorSets = nullptr;
	
		vk::raii::CommandBuffers* mCommandBuffers = nullptr;

	};
} // namespace BHive