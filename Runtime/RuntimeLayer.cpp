#include "RuntimeLayer.h"

#include "core/Application.h"

#include "core/layers/ImGuiLayer.h"
#include "core/platform/Platform.h"
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
#include "mesh/MeshImporter.h"
#include "mesh/MeshImportResolver.h"
#include "mesh/StaticMesh.h"
#include "renderers/LineRenderer.h"

namespace BHive
{

	struct UniformBufferObject
	{
		alignas(16) glm::mat4 proj;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 model;
	};

	void RuntimeLayer::OnAttach()
	{
		CreateUniformBuffers();
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

		auto current_frame = swap_chain->GetCurrentFrame();

		RenderCommand::ClearColor(0.01f, 0.01f, 0.01f, 1.f);
		RenderCommand::SetViewport(0, 0, size.x, size.y);

		LineRenderer::Begin();
		LineRenderer::DrawLine({0, 0, 0}, {1, 1, 1}, FColor::Green);
		LineRenderer::End();

		if (mGraphicsPipeline)
		{
			auto &sets = mMaterial->GetDescriptorSets();
			mUniformBuffer->WriteDescriptor(sets[current_frame]);
			mMaterial->Submit(mShader);

			auto *api = RenderCommand::GetAPI();
			api->BindPipeline(*mGraphicsPipeline);

			UpdateUniformBuffer();

			api->BindDescriptorSets(mPipelineLayout, sets);

			if (mMesh)
				api->DrawElements(EDrawMode::Triangles, *mMesh->GetVertexArray());
		}
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

		auto texture_id = ImGuiLayer::GetTextureID(mTexture);
		ImGui::Image(texture_id, {200, 200}, {0, 1}, {1, 0});

		if (ImGui::Button("Load Mesh"))
		{
			auto info = Platform::OpenFile("Mesh (*.glb;*.gltf)\0*.glb;*.gltf\0");
			if (info)
			{
				FMeshImportData import_data{};
				FMeshImportOptions import_options{};

				if (MeshImporter::Import(info, import_data))
				{
					std::vector<Ref<Asset>> additional_assets;
					MeshImportResolver resolver(import_data, import_options, additional_assets);
					mMesh = Cast<StaticMesh>(resolver.Resolve());

					if (mMesh)
					{
						const auto &bindingDescription = mMesh->GetVertexArray()->GetBindingDescription();
						const auto &attributeDescriptions = mMesh->GetVertexArray()->GetAttributeDescriptions();

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
				}
			}
		}

		ImGui::End();

		// GUI::EndDockSpace();
	}

	void RuntimeLayer::CreateUniformBuffers()
	{
		mUniformBuffer = CreateRef<UniformBuffer>(0, sizeof(UniformBufferObject));
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
	}

} // namespace BHive