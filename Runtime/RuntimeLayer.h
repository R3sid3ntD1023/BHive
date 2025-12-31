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

		void CreateDescriptors();

		void UpdateUniformBuffer();

		void CreateGraphicsPipeline();

	private:
		Ref<Texture2D> mTexture = nullptr;

		Ref<VertexBuffer> mVertexBuffer = nullptr;

		Ref<IndexBuffer> mIndexBuffer = nullptr;

		Ref<VertexArray> mVertexArray = nullptr;

		Ref<VulkanPipeline> mGraphicsPipeline = nullptr;

		vk::raii::PipelineLayout mPipelineLayout = nullptr;

		Ref<UniformBuffer> mUniformBuffer;
		Ref<class Shader> mShader;
		Ref<class Material> mMaterial;
	};
} // namespace BHive