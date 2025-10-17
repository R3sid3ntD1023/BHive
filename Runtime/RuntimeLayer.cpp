#include "RuntimeLayer.h"

#include "core/Application.h"
#include "gfx/Buffers.h"
#include "gfx/GraphicsContext.h"
#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "gfx/UniformBuffer.h"
#include "gfx/VertexArray.h"
#include "gfx/VulkanPipeline.h"
#include "gfx/VulkanSwapChain.h"
#include "gui/Gui.h"
#include "importers/TextureImporter.h"

namespace BHive
{
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Color;
		glm::vec2 TexCoord;

		static BufferLayout GetLayout() { return {{EShaderDataType::Float3}, {EShaderDataType::Float3}, {EShaderDataType::Float2}}; };
	};

	struct UniformBufferObject
	{
		alignas(16) glm::mat4 proj;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 model;
	};

	static const std::vector<Vertex> sVertices = {
		{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.f, 0.0f}},
		{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.f, 0.0f}},
		{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.f, 1.0f}},
		{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.f, 1.0f}}};

	static const std::vector<uint32_t> sIndices = {0, 1, 2, 2, 3, 0};

	void RuntimeLayer::OnAttach()
	{

		CreateIndexBuffer();
		CreateVertexBuffer();
		CreateVertexArray();
		CreateTextureImage();
		CreateUniformBuffers();

		CreateDescriptorPool();
		CreateDescriptorSetLayout();
		CreateDescriptorSets();
		CreateGraphicsPipeline();

	}

	void RuntimeLayer::OnDetach()
	{
	
	}

	void RuntimeLayer::OnUpdate(float)
	{
		auto &app = Application::Get();
		auto &window = app.GetWindow();
		auto size = window.GetSize();
		auto &context = GraphicsContext::Get();
		auto &swap_chain = context.GetSwapChain();
		auto extents = swap_chain->GetExtent();
		auto current_frame = swap_chain->GetCurrentFrame();

		auto *api = RenderCommand::GetAPI();
		api->BindPipeline(*mGraphicsPipeline);

		RenderCommand::ClearColor(0.f, 0.f, 0.f);
		RenderCommand::SetViewport(0, 0, extents.width, extents.height);
		UpdateUniformBuffer(current_frame);

		api->BindDescriptorSets(mPipelineLayout, mDescriptorSets);
		api->DrawElements(EDrawMode::Triangles, *mVertexArray);

		
	}

	void RuntimeLayer::OnGuiRender()
	{
		static float value = 0.f;

		GUI::BeginDockSpace("Dockspace");

		if (ImGui::BeginMainMenuBar())
		{
			ImGui::EndMainMenuBar();
		}

		if (ImGui::Begin("Window"))
		{
			ImGui::DragFloat("Test", &value);
		}

		ImGui::End();

		GUI::EndDockSpace();
	}

	void RuntimeLayer::CreateTextureImage()
	{
		mTexture = TextureLoader::Import("C:/Users/dariu/Documents/BHive/projects/Mario/resources/sprites0.jpg", {});
	}

	void RuntimeLayer::CreateVertexBuffer()
	{
		mVertexBuffer = CreateRef<VertexBuffer>(sVertices.size() * sizeof(Vertex));
		mVertexBuffer->SetData(sVertices.data(), sVertices.size() * sizeof(Vertex));
		mVertexBuffer->SetLayout(Vertex::GetLayout());
	}

	void RuntimeLayer::CreateIndexBuffer()
	{
		mIndexBuffer = CreateRef<IndexBuffer>(sIndices.data(), (uint32_t)sIndices.size());
		
	}

	void RuntimeLayer::CreateVertexArray()
	{
		mVertexArray = CreateRef<VertexArray>();
		mVertexArray->AddVertexBuffer(mVertexBuffer);
		mVertexArray->SetIndexBuffer(mIndexBuffer);
	}

	void RuntimeLayer::CreateUniformBuffers()
	{
		mUniformBuffers.clear();

		for (size_t i = 0; i < 2; i++)
		{
			mUniformBuffers.emplace_back(CreateRef<UniformBuffer>(0, sizeof(UniformBufferObject)));
		}
	}

	void RuntimeLayer::UpdateUniformBuffer(uint32_t currentImage)
	{
		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		auto &app = Application::Get();
		auto& window = app.GetWindow();
		auto aspect = window.GetAspectRatio();

		UniformBufferObject ubo{};
		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 10.0f);
		ubo.proj[1][1] *= -1; // for vulkan coordinate system

		mUniformBuffers[currentImage]->SetData(&ubo, sizeof(ubo));
	}

	void RuntimeLayer::CreateDescriptorSetLayout()
	{
		auto &context = GraphicsContext::Get();
		auto& device = context.GetDevice();

		std::array bindings = {
			vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex, nullptr),
			vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr)};
		vk::DescriptorSetLayoutCreateInfo layoutInfo({}, bindings);
		mDescriptorSetLayout = device.createDescriptorSetLayout(layoutInfo);
	}

	void RuntimeLayer::CreateDescriptorPool()
	{
		auto &context = GraphicsContext::Get();
		auto &device = context.GetDevice();

		std::array poolSize = {
			vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, 2), vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, 2)};

		vk::DescriptorPoolCreateInfo poolInfo(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, 2, poolSize);

		mDescriptorPool = device.createDescriptorPool(poolInfo);
	}

	void RuntimeLayer::CreateDescriptorSets()
	{
		auto &context = GraphicsContext::Get();
		auto &device = context.GetDevice();

		std::vector<vk::DescriptorSetLayout> layouts(2, *mDescriptorSetLayout);
		vk::DescriptorSetAllocateInfo allocInfo(*mDescriptorPool, layouts);

		mDescriptorSets = device.allocateDescriptorSets(allocInfo);

		for (size_t i = 0; i < 2; i++)
		{
			const vk::raii::Buffer &buffer = *mUniformBuffers[i];
			vk::DescriptorBufferInfo bufferInfo(buffer, 0, sizeof(UniformBufferObject));
			vk::DescriptorImageInfo image_info(mTexture->GetSampler(), mTexture->GetView(), vk::ImageLayout::eShaderReadOnlyOptimal);

			std::array descriptorWrites = {
				vk::WriteDescriptorSet(mDescriptorSets[i], 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &bufferInfo, nullptr),
				vk::WriteDescriptorSet(mDescriptorSets[i], 1, 0, 1, vk::DescriptorType::eCombinedImageSampler, &image_info)};
			device.updateDescriptorSets(descriptorWrites, nullptr);
		}
	}

	void RuntimeLayer::CreateGraphicsPipeline()
	{
		auto shader = ShaderManager::Get().Load("C:/Users/dariu/Documents/BHive/Runtime/Triangle.glsl");

		auto &context = GraphicsContext::Get();
		auto &device = context.GetDevice();
		auto &swap_chain = context.GetSwapChain();

		const auto &bindingDescription = mVertexArray->GetBindingDescription();
		const auto &attributeDescriptions = mVertexArray->GetAttributeDescriptions();

		vk::PipelineLayoutCreateInfo pipeline_layout_create_info{};
		pipeline_layout_create_info.setLayoutCount = 1;
		pipeline_layout_create_info.pSetLayouts = &*mDescriptorSetLayout;
		pipeline_layout_create_info.pushConstantRangeCount = 0;
		mPipelineLayout = device.createPipelineLayout(pipeline_layout_create_info);

		vk::PipelineRenderingCreateInfo pipeline_renderingCreateInfo{};
		pipeline_renderingCreateInfo.colorAttachmentCount = 1;
		pipeline_renderingCreateInfo.pColorAttachmentFormats = &swap_chain->GetFormat().format;

		FPipelineConfigInfo config = VulkanPipeline::GetDefaultConfigInfo(swap_chain->GetWidth(), swap_chain->GetHeight());
		config.Layout = mPipelineLayout;
		config.Next = &pipeline_renderingCreateInfo;
		config.InputState = vk::PipelineVertexInputStateCreateInfo({}, bindingDescription, attributeDescriptions);

		mGraphicsPipeline = CreateRef<VulkanPipeline>();
		mGraphicsPipeline->Init(device, {shader}, config);
	}

}