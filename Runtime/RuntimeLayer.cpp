#include "RuntimeLayer.h"

#include "core/Application.h"
#include "core/layers/ImGuiLayer.h"
#include "gfx/Buffers.h"
#include "gfx/DescriptorBuilder.h"
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
#include "material/Material.h"

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
		CreateUniformBuffers();

		CreateDescriptors();
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

		auto &sets = mMaterial->GetDescriptorSets();
		mUniformBuffer->WriteDescriptor(sets[current_frame]);
		mMaterial->Submit(mShader);

		auto *api = RenderCommand::GetAPI();
		api->BindPipeline(*mGraphicsPipeline);

		RenderCommand::ClearColor(0.01f, 0.01f, 0.01f, 1.f);
		RenderCommand::SetViewport(0, 0, extents.width, extents.height);
		UpdateUniformBuffer();

		api->BindDescriptorSets(mPipelineLayout, sets);
		api->DrawElements(EDrawMode::Triangles, *mVertexArray);
	}

	void RuntimeLayer::OnGuiRender()
	{
		static float value = 0.f;

		/*GUI::BeginDockSpace("Dockspace");

		if (ImGui::BeginMainMenuBar())
		{
			ImGui::EndMainMenuBar();
		}*/

		if (ImGui::Begin("Window"))
		{
			ImGui::DragFloat("Test", &value);
		}

		auto &context = GraphicsContext::Get();
		auto &device = VulkanCore::GetLogicalDevice();
		auto &swap_chain = context.GetSwapChain();
		auto image_count = swap_chain->GetImageCount();
		auto current_frame = swap_chain->GetCurrentFrame();
		auto &app = Application::Get();
		auto imgui_layer = app.GetImGuiLayer();

	/*	auto image_info = reinterpret_cast<const vk::DescriptorImageInfo *>(mTexture->GetNativeHandle());
		FDescriptorWriter(imgui_layer->GetDescriptorSetLayout(), imgui_layer->GetDescriptorPool())
			.WriteImage(0, *image_info).Overwrite(imgui_layer->GetDescriptorSets());

		ImTextureRef texture(*imgui_layer->GetDescriptorSet(current_frame));
		ImGui::Image(texture, {200, 200}, {0, 1}, {1, 0});*/

		ImGui::End();

		//GUI::EndDockSpace();
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
		mUniformBuffer = CreateRef<UniformBuffer>(0, sizeof(UniformBufferObject));
	}

	void RuntimeLayer::CreateDescriptors()
	{
		/*mDescriptorSetLayout = FDescriptorSetLayout::Builder()
								   .AddBinding(0, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex, 1)
								   .AddBinding(1, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment, 1)
								   .Build();

		mDescriptorPool = FDescriptorPool::Builder().SetMaxSets(2).AddPoolSize(vk::DescriptorType::eUniformBuffer, 2).AddPoolSize(vk::DescriptorType::eCombinedImageSampler, 2).Build();

		auto buffer_info = reinterpret_cast<const vk::DescriptorBufferInfo *>(mUniformBuffer->GetNativeHandle());
		auto image_info = reinterpret_cast<const vk::DescriptorImageInfo *>(mTexture->GetNativeHandle());
		FDescriptorWriter(mDescriptorSetLayout, mDescriptorPool).WriteBuffer(0, *buffer_info).WriteImage(1, *image_info).Build(mDescriptorSets);*/
	}

	void RuntimeLayer::UpdateUniformBuffer()
	{
		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		auto &app = Application::Get();
		auto &window = app.GetWindow();
		auto aspect = window.GetAspectRatio();

		UniformBufferObject ubo{};
		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 10.0f);
		ubo.proj[1][1] *= -1; // for vulkan coordinate system

		mUniformBuffer->SetData(&ubo, sizeof(ubo));
	}

	void RuntimeLayer::CreateGraphicsPipeline()
	{
		mShader = ShaderManager::Get().Load("C:/Users/dariu/Documents/BHive/Runtime/Triangle.glsl");
		mTexture = TextureLoader::Import("C:/Users/dariu/Documents/BHive/projects/Mario/resources/sprites0.jpg", {});
		mMaterial = CreateRef<Material>(mShader);
		mMaterial->SetTexture("u_Texture", mTexture);

		auto &context = GraphicsContext::Get();
		auto &device = VulkanCore::GetLogicalDevice();
		auto &swap_chain = context.GetSwapChain();
		auto api = RenderCommand::GetAPI();

		const auto &bindingDescription = mVertexArray->GetBindingDescription();
		const auto &attributeDescriptions = mVertexArray->GetAttributeDescriptions();

		vk::PipelineLayoutCreateInfo pipeline_layout_create_info({}, *mMaterial->GetDescriptorSetLayout()->GetLayout());
		mPipelineLayout = device.createPipelineLayout(pipeline_layout_create_info);

		vk::PipelineRenderingCreateInfo pipeline_renderingCreateInfo({}, {swap_chain->GetFormat().format});

		FPipelineConfigInfo config = VulkanPipeline::GetDefaultConfigInfo(swap_chain->GetWidth(), swap_chain->GetHeight());
		config.Layout = mPipelineLayout;
		config.Next = &pipeline_renderingCreateInfo;
		config.InputState = vk::PipelineVertexInputStateCreateInfo({}, bindingDescription, attributeDescriptions);

		mGraphicsPipeline = CreateRef<VulkanPipeline>();
		mGraphicsPipeline->Init(device, {mShader}, config);
	}

} // namespace BHive