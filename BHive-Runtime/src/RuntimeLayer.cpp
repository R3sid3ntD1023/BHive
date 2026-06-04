#include "RuntimeLayer.h"

#include "core/Application.h"

#include "core/layers/ImGuiLayer.h"
#include "core/platform/Platform.h"
#include "gfx/RenderCommand.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "gui/Gui.h"
#include "importers/TextureImporter.h"
#include "gfx/material/Material.h"
#include "gfx/material/EmissiveMaterial.h"
#include "importers/MeshImporter.h"
#include "importers/MeshImportResolver.h"
#include "gfx/mesh/StaticMesh.h"
#include "gfx/renderers/Renderer.h"
#include "gfx/Pipeline.h"
#include "core/Time.h"
#include "Inspectors/Inspect.h"
#include "gfx/Framebuffer.h"
#include "gfx/ISetManager.h"
#include "gfx/GlobalBuffers.h"
#include "gfx/material/LambertMaterial.h"
#include "gfx/debug/ImageDebugger.h"

namespace BHive
{
	FTransform transform{};

	void RuntimeLayer::OnAttach(Application& app)
	{
		auto triangleShader = ShaderManager::Get("Triangle.glsl");
		//mEmissiveShader = ShaderManager::Get().Load(ENGINE_SHADER_PATH "/Emissive.glsl");
		
		mEnvironmentTex = TextureLoader::Import(ENGINE_PATH"/data/hdr/kloofendal_43d_clear_puresky_1k.hdr");
		Renderer::Get().SetEnvironmentTexture(mEnvironmentTex);

		auto state = Pipeline::GetDefaultGraphicsPipelineState();
		state.ShaderProgram = triangleShader;
		state.ColorAttachmentFormats = {EFormat::RGBA8};
		
		PipelineRegistry::Register("Triangle", state);
		
		mTexture = TextureLoader::Import("C:/Users/dariu/Documents/BHive/projects/Mario/resources/sprites0.jpg", {});
		mMaterial = CreateRef<Material>();
		mMaterial->SetPipeline(PipelineRegistry::Get("Triangle"));

		mMaterial->SetTexture("u_Texture", mTexture);
		mMaterial->Set("u_Color", glm::vec3(1, 1, 1));

		/*mEmmissivePipeline = Pipeline::Create();
		state.ShaderProgram = mEmissiveShader;
		mEmmissivePipeline->Init(state);
		mEmissiveMaterial = CreateRef<EmissiveMaterial>(mEmmissivePipeline);
		mEmissiveMaterial->EmissionColor = FColor::Green;*/

		/*mShader = ShaderManager::Get().Load(ENGINE_SHADER_PATH"/Lambert.glsl");
		mPipeline = Pipeline::Create();
		state.ShaderProgram = mShader;
		mPipeline->Init(state);
		mLambertMaterial = CreateRef<LambertMaterial>(mPipeline);
		mLambertMaterial->DiffuseColor = FColor::Purple;
		mLambertMaterial->EmissionColor = FColor::Yellow;
		mLambertMaterial->SetTexture("DiffuseMap", mTexture);*/

		// create mesh
		FMeshImportData import_data{};
		FMeshImportOptions import_options{.ImportMaterials = false};

		if (MeshImporter::Import("C:/Users/dariu/Documents/Cube.glb", import_data))
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

		FramebufferSpecification fb_specs;
		fb_specs.Size = window.GetSize();
		fb_specs.Attachments.attach(FTextureCreateInfo{
				.Format = EFormat::RGBA8, .WrapMode = EWrapMode::CLAMP_TO_EDGE})
			.attach(FTextureCreateInfo{.Format = EFormat::DEPTH24_STENCIL8, .WrapMode = EWrapMode::CLAMP_TO_EDGE});

		mFramebuffer = Framebuffer::Create(fb_specs);
		
		std::vector<MultiDrawIndirectCommand> commands;

		for (size_t i = 0; i < 2; i++)
		{
			for (auto &m : mMesh->GetSubMeshes())
			{
				MultiDrawIndirectCommand cmd{};
				cmd.Count = m.IndexCount;
				cmd.BaseInstance = 0;
				cmd.BaseVertex = m.StartVertex;
				cmd.FirstIndex = m.StartIndex;
				cmd.InstanceCount = 1;
				commands.emplace_back(cmd);
			}
		}

		mMultiDrawIndirectBuffer = GPUBuffer::Create(sizeof(MultiDrawIndirectCommand) * commands.size(), EBufferType::IndirectBuffer);
		mMultiDrawIndirectBuffer->SetData(commands.data(), sizeof(MultiDrawIndirectCommand) * commands.size());
		
		auto &dbg = ImageDebugger::Get();
		dbg.Initialize({512, 512});

		dbg.RegisterTexture("PreFilterEnv", Renderer::Get().GetPreFilterEnvironmentTexture());
		dbg.RegisterTexture("EnvironmentCube", Renderer::Get().GetEnviromentCubeTexture());
		dbg.RegisterTexture("Irradiance", Renderer::Get().GetIrradianceTexture());
		dbg.RegisterTexture("BRDFLUT", Renderer::Get().GetBRDFLUTTexture());
		dbg.RegisterTexture("Test", mTexture);
	}

	void RuntimeLayer::OnDetach()
	{
	}

	void RuntimeLayer::OnUpdate(float time)
	{	
		transform.AddRotation({0, time * 10.f, 0});

		mCamera.ProcessInput();
	}

	void RuntimeLayer::OnRender(Renderer& renderer)
	{
		auto &app = Application::Get();
		auto &window = app.GetWindow();
		auto size = window.GetSize();


		renderer.BeginPass("Scene", EPassType::OffScreen);
		mFramebuffer->Bind();

	
		renderer.ClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		renderer.Clear();
		renderer.SubmitCamera(mCamera.GetProjection(), mCamera.GetView());

		renderer.Line.DrawLine({-1, 2, 0}, {1, 2, 0}, FColor::Green);
		renderer.Line.DrawGrid({});
		renderer.Line.DrawBox(glm::vec3{1.f}, glm::vec3{0.0f}, FColor::Blue, transform);
		

		FQuadParams params{.Size = {1, 1}, .Color = FColor::Red};
		renderer.Quad.DrawQuad(params, nullptr, FTransform({0, 0, 2}));

		params.Color = FColor::White;
		renderer.Quad.DrawQuad(params, mTexture, FTransform({0, 0, -2}));

		renderer.Quad.DrawCircle({.Radius = 1.f, .LineColor = FColor::Orange}, FTransform({2, 0, 0}));

		FTextParams tex_params{};
		renderer.Quad.DrawText(1.0f, "Cube", tex_params, FTransform({0, 2, 0}));
		
		renderer.Flush();

		if (mMesh && mMaterial)
		{		
			mMaterial->Set("u_Time", Time::Raw());
			mMaterial->Submit();

			renderer.GetModelBuffer().Reset();
			renderer.GetModelBuffer().Submit(transform);
			renderer.GetModelBuffer().Submit(FTransform({3, 4, 0}));
			renderer.GetModelBuffer().Upload();

			renderer.MultiDrawElementsIndirect(ETopologyMode::Triangles, mMultiDrawIndirectBuffer.get(), mMesh->GetVertexArray().get(), 2, sizeof(MultiDrawIndirectCommand));
		}

		mFramebuffer->UnBind();

		renderer.EndPass();

		ImageDebugger::Get().OnRender(renderer);

	}

	void RuntimeLayer::OnGuiRender()
	{
		GUI::BeginDockSpace("Dockspace");

		ImageDebugger::Get().OnGuiRender();

		if (ImGui::Begin("Scene"))
		{
			if (mFramebuffer)
			{
				auto fbSize = mFramebuffer->GetSize();
				auto viewportSize = ImGui::GetContentRegionAvail();
				glm::uvec2 size = {viewportSize.x, viewportSize.y};

				if (size != fbSize && size.x != 0 && size.y != 0)
				{
					mFramebuffer->Resize(size);
				}
				auto texture_id = ImGuiLayer::GetTextureID(*mFramebuffer->GetColorAttachment(0));
				ImGui::Image(texture_id, viewportSize);
			}
		}

		ImGui::End();

		if (ImGui::Begin("Window"))
		{
			auto texture_id = ImGuiLayer::GetTextureID(*mTexture);
			ImGui::Image(texture_id, {200, 200}, {0, 1}, {1, 0});

			rttr::variant var = transform;

			if (Inspect::get().inspect({}, var))
				transform = var.get_value<FTransform>();

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

			if (ImGui::Button("Load HDR"))
			{
				auto info = Platform::OpenFile("HDR (*.hdr;)\0*.hdr;\0");
				if (info)
				{
					auto tex = TextureLoader::Import(info);
					Renderer::Get().SetEnvironmentTexture(tex);
				}
			}
		}

		ImGui::End();

		GUI::EndDockSpace();
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