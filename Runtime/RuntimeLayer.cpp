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
#include "renderers/Renderer.h"

namespace BHive
{

	void RuntimeLayer::OnAttach()
	{
		CreateGraphicsPipeline();

		mCamera.SetView(FTransform({0, 1, -5}));
	}

	void RuntimeLayer::OnDetach()
	{
	}

	void RuntimeLayer::OnUpdate(float)
	{
		auto &app = Application::Get();
		auto &window = app.GetWindow();
		auto size = window.GetSize();
		auto aspect = window.GetAspectRatio();

		auto &context = GraphicsContext::Get();
		auto &swap_chain = context.GetSwapChain();

		auto current_frame = swap_chain->GetCurrentFrame();

		

		RenderCommand::ClearColor(0.01f, 0.01f, 0.01f, 1.f);
		RenderCommand::SetViewport(0, 0, size.x, size.y);

		Renderer::Begin();

		Renderer::SubmitCamera(mCamera.GetProjection(), mCamera.GetView());
		LineRenderer::DrawLine({-1, 0, 0}, {1, 0, 0}, FColor::Green);
		
		if (mMaterial)
		{
	/*		auto &sets = mMaterial->GetDescriptorSets();
			uniform_buffer->WriteDescriptor(sets[current_frame]);

	*/
			mMaterial->Submit(mShader);

			if (mMesh)
				RenderCommand::DrawElements(EDrawMode::Triangles, *mMesh->GetVertexArray());
		}

		Renderer::End();
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
				}
			}
		}

		ImGui::End();

		// GUI::EndDockSpace();
	}


	void RuntimeLayer::CreateGraphicsPipeline()
	{
		mShader = ShaderManager::Get().Load("C:/Users/dariu/Documents/BHive/Runtime/Triangle.glsl");
		mTexture = TextureLoader::Import("C:/Users/dariu/Documents/BHive/projects/Mario/resources/sprites0.jpg", {});
		mMaterial = CreateRef<Material>(mShader);
		mMaterial->SetTexture("u_Texture", mTexture);
	}

} // namespace BHive