#include "gfx/DescriptorBuilder.h"
#include "gfx/GraphicsContext.h"
#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "gfx/UniformBuffer.h"
#include "gfx/VulkanPipeline.h"
#include "gfx/VulkanSwapChain.h"
#include "LineRenderBatch.h"
#include "renderers/Renderer.h"

namespace BHive
{
	void LineRenderBatch::Init()
	{
		mVertexDataBuffer = new FLineVertex[sMaxVertexCount];

		mVertexBuffer = CreateRef<VertexBuffer>(sMaxVertexCount * sizeof(FLineVertex));
		mVertexBuffer->SetLayout({{EShaderDataType::Float3}, {EShaderDataType::Float4}, {EShaderDataType::Int}});

		mVertexArray = CreateRef<VertexArray>();
		mVertexArray->AddVertexBuffer(mVertexBuffer);

		mLineShader = ShaderManager::Get().Load(ENGINE_SHADER_PATH "/Line.glsl");

		const auto &bindingDescription = mVertexArray->GetBindingDescription();
		const auto &attributeDescriptions = mVertexArray->GetAttributeDescriptions();

		auto &device = VulkanCore::GetLogicalDevice();
		auto &swap_chain = GraphicsContext::Get().GetSwapChain();

		mDescriptorSetLayout = FDescriptorSetLayout::Builder().AddBinding(0, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex, 1).Build();
		mDescriptorPool = FDescriptorPool::Builder().AddPoolSize(vk::DescriptorType::eUniformBuffer, 1).Build();

		vk::PipelineLayoutCreateInfo pipeline_layout_create_info({}, *mDescriptorSetLayout->GetLayout());
		mPipelineLayout = device.createPipelineLayout(pipeline_layout_create_info);

		vk::PipelineRenderingCreateInfo pipeline_renderingCreateInfo({}, {swap_chain->GetFormat().format});

		FPipelineConfigInfo config = VulkanPipeline::GetDefaultConfigInfo(swap_chain->GetWidth(), swap_chain->GetHeight());
		config.Layout = mPipelineLayout;
		config.Next = &pipeline_renderingCreateInfo;
		config.InputState = vk::PipelineVertexInputStateCreateInfo({}, bindingDescription, attributeDescriptions);

		mGraphicsPipeline = CreateRef<VulkanPipeline>();
		mGraphicsPipeline->Init(device, {mLineShader}, config);

		FDescriptorWriter(mDescriptorSetLayout, mDescriptorPool).Build(mDescriptorSets);
	}

	void LineRenderBatch::End()
	{
		mLineShader->Bind();
		Flush();
		mLineShader->UnBind();
	}

	void LineRenderBatch::NextBatch()
	{
		if (mVertexCount >= sMaxVertexCount)
		{
			End();
			StartBatch();
		}
	}

	void LineRenderBatch::StartBatch()
	{
		mVertexDataPtr = mVertexDataBuffer;
		mVertexCount = 0;
	}

	void LineRenderBatch::Flush()
	{
		if (mVertexCount > 0)
		{
			auto &context = GraphicsContext::Get();
			auto &swap_chain = context.GetSwapChain();

			auto current_frame = swap_chain->GetCurrentFrame();
			Renderer::GetCamera().GetUniformBuffer()->WriteDescriptor(mDescriptorSets[current_frame]);

			auto api = RenderCommand::GetAPI();

			api->BindPipeline(*mGraphicsPipeline);
			api->BindDescriptorSets(mPipelineLayout, mDescriptorSets);

			uint32_t size = (uint32_t)((uint8_t *)mVertexDataPtr - (uint8_t *)mVertexDataBuffer);
			mVertexBuffer->SetData(mVertexDataBuffer, size);

			RenderCommand::DrawArrays(Lines, *mVertexArray, mVertexCount);
			Renderer::GetStats().DrawCalls++;
		}
	}

	LineRenderBatch::~LineRenderBatch()
	{
		mVertexDataPtr = nullptr;
		delete[] mVertexDataBuffer;
	}
} // namespace BHive