#pragma once


#include "core/Core.h"
#include "core/Layer.h"
#include "gfx/VulkanCore.h"

namespace BHive
{
	class Texture2D;
	class VertexBuffer;
	class IndexBuffer;
	class VertexArray;
	class UniformBuffer;
	class VulkanPipeline;

	class RuntimeLayer : public Layer
	{
	public:

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(float) override;
		void OnGuiRender() override;

	private:
		void CreateVertexBuffer();

		void CreateIndexBuffer();

		void CreateVertexArray();

		void CreateUniformBuffers();

		void CreateDescriptorSetLayout();

		void CreateDescriptorPool();

		void CreateDescriptorSets();

		void CreateTextureImage();

		void UpdateUniformBuffer(uint32_t currentImage);

		void CreateGraphicsPipeline();

	private:
		Ref<Texture2D> mTexture = nullptr;

		Ref<VertexBuffer> mVertexBuffer = nullptr;

		Ref<IndexBuffer> mIndexBuffer = nullptr;

		Ref<VertexArray> mVertexArray = nullptr;

		Ref<VulkanPipeline> mGraphicsPipeline = nullptr;

		vk::raii::DescriptorSetLayout mDescriptorSetLayout = nullptr;

		vk::raii::PipelineLayout mPipelineLayout = nullptr;

		std::vector<Ref<UniformBuffer>> mUniformBuffers;

		vk::raii::DescriptorPool mDescriptorPool = nullptr;

		std::vector<vk::raii::DescriptorSet> mDescriptorSets;

	};
}