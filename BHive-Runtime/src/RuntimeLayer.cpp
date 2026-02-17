#include "RuntimeLayer.h"

#include "core/Application.h"

#include "core/layers/ImGuiLayer.h"
#include "core/platform/Platform.h"
#include "gfx/RenderCommand.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "gui/Gui.h"
#include "importers/TextureImporter.h"
#include "material/Material.h"
#include "mesh/MeshImporter.h"
#include "mesh/MeshImportResolver.h"
#include "mesh/StaticMesh.h"
#include "renderers/Renderer.h"
#include "gfx/Pipeline.h"
#include "core/Time.h"

namespace BHive
{

	void RuntimeLayer::OnAttach(Application& app)
	{
		mShader = ShaderManager::Get().Load(ENGINE_SHADER_PATH "/Triangle.glsl");

		Pipeline::PipelineState state = Pipeline::GetDefaultPipelineState();
		state.ShaderProgram = mShader;
		state.ColorAttachmentFormats = {EFormat::RGBA8};

		mPipeline = Pipeline::Create();
		mPipeline->Init(state);

		mTexture = TextureLoader::Import("C:/Users/dariu/Documents/BHive/projects/Mario/resources/sprites0.jpg", {});
		mMaterial = CreateRef<Material>(mPipeline);
		mMaterial->SetTexture("u_Texture", mTexture);
		mMaterial->Set("u_Color", glm::vec3(1, .5f, 0));

		// create mesh
		FMeshImportData import_data{};
		FMeshImportOptions import_options{};

		if (MeshImporter::Import("C:/Users/dariu/Documents/Cube.glb ", import_data))
		{
			std::vector<Ref<Asset>> additional_assets;
			MeshImportResolver resolver(import_data, import_options, additional_assets);
			mMesh = Cast<StaticMesh>(resolver.Resolve());
		}
		auto &window = app.GetWindow();
		auto aspect = window.GetAspectRatio();

		mCamera = EditorCamera(75.f, aspect, 0.1f, 1000.f);
		mCamera.SetView(FTransform({5, 5, 5}));
		mCamera.Focus(FTransform({0, 0, 0}));
	}

	void RuntimeLayer::OnDetach()
	{
	}

	void RuntimeLayer::OnUpdate(float time)
	{	
		static FTransform transform{};
		transform.AddRotation({0, time * 10.f, 0});

		mCamera.ProcessInput();

		auto &app = Application::Get();
		auto &window = app.GetWindow();
		auto size = window.GetSize();

		RenderCommand::ClearColor(0.1f, 0.1f, 0.1f, 1.f);
		RenderCommand::Clear();
		RenderCommand::SetViewport(0, 0, size.x, size.y);

		Renderer::Begin();

		Renderer::SubmitCamera(mCamera.GetProjection(), mCamera.GetView());

		LineRenderer::DrawLine({-1, 2, 0}, {1, 2, 0}, FColor::Green);
		LineRenderer::DrawGrid({});
		LineRenderer::DrawBox(glm::vec3{1.f}, glm::vec3{0.0f}, FColor::Blue, transform);

		if (mMesh && mMaterial)
		{
			mMaterial->Submit();
			mMaterial->Set("u_Time", Time::Raw());
	
			Renderer::SubmitModel(transform);

			if (mMesh)
				RenderCommand::DrawElements(ETopologyMode::Triangles, mMesh->GetVertexArray());
		}


		Renderer::End();
	}

	void RuntimeLayer::OnGuiRender()
	{
		static float value = 0.f;

		if (ImGui::Begin("Window"))
		{
			ImGui::DragFloat("Test", &value);
		}

		auto texture_id = ImGuiLayer::GetTextureID(*mTexture);
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

	void RuntimeLayer::OnEvent(Event &e)
	{
		mCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch(this, &RuntimeLayer::OnWindowResize);
	}

	bool RuntimeLayer::OnWindowResize(WindowResizeEvent &e)
	{
		mCamera.Resize(e.x , e.y);

		return false;
	}


} // namespace BHive