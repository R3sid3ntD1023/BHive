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
#include "gfx/material/LambertMaterial.h"
#include "gfx/debug/ImageDebugger.h"
#include "gfx/cameras/OrthographicCamera.h"
#include "gfx/mesh/primitives/Sphere.h"

namespace BHive
{
	FTransform transform{{6.f, 0.f, 0.f}};

	struct FPerObjectData
	{
		glm::mat4 WorldMatrix = {1.0f};
	};

	const uint32_t objectCount = 3;

	static FDirectionalLight mainLight{.Color = {1.f, 1.f, 1.f, 1.f}, .Direction = {-1.f, -.6f, 0.f, 0.f}};
	static FPointLight plight0{.Color = {1.f, .5f, 0.f, 4.f}, .Position = {0.f, 2.f, 0.f, 10000.f}};

	void RuntimeLayer::OnAttach(Application& app)
	{
		mEnvironmentTex = TextureLoader::Import(ENGINE_PATH"/data/hdr/kloofendal_43d_clear_puresky_1k.hdr");
		Renderer::Get().SetEnvironmentTexture(mEnvironmentTex);

		{
			auto triangleShader = ShaderManager::Get("Triangle.glsl");
			auto state = Pipeline::GetDefaultGraphicsPipelineState();
			state.ShaderProgram = triangleShader;
			state.ColorAttachmentFormats = {EFormat::RGBA32F};
			PipelineRegistry::Register("Triangle", state);
		}

		{
			auto emissiveShader = ShaderManager::Get("Emissive.glsl");
			auto state = Pipeline::GetDefaultGraphicsPipelineState();
			state.ShaderProgram = emissiveShader;
			state.ColorAttachmentFormats = {EFormat::RGBA32F};
			PipelineRegistry::Register("Emissive", state);
		}

		{
			auto lambert = ShaderManager::Get("Lambert.glsl");
			auto state = Pipeline::GetDefaultGraphicsPipelineState();
			state.ShaderProgram = lambert;
			state.ColorAttachmentFormats = {EFormat::RGBA32F};
			PipelineRegistry::Register("Lambert", state);
		}
		
		mTexture = TextureLoader::Import("C:/Users/dariu/Documents/BHive/projects/Mario/resources/sprites0.jpg", {});

		// create mesh
		FMeshImportData import_data{};
		FMeshImportOptions import_options{.ImportMaterials = false};

		if (MeshImporter::Import("C:/Users/dariu/Documents/Cube.glb", import_data))
		{
			std::vector<Ref<Asset>> additional_assets;
			MeshImportResolver resolver(import_data, import_options, additional_assets);
			mMesh = Cast<StaticMesh>(resolver.Resolve());
		}

		mSphere = CreateRef<PSphere>(1.0f);
		
		//create model buffers
		{
			std::vector<MultiDrawIndirectCommand> commands;

			size_t i = 0;
			for (i; i < objectCount; i++)
			{
				for (auto &m : mMesh->GetSubMeshes())
				{
					MultiDrawIndirectCommand cmd{};
					cmd.Count = m.IndexCount;
					cmd.BaseInstance = i;
					cmd.BaseVertex = m.StartVertex;
					cmd.FirstIndex = m.StartIndex;
					cmd.InstanceCount = 1;
					commands.emplace_back(cmd);
				}
			}

			for (i; i < objectCount + 2; i++)
			{
				for (auto &m : mSphere->GetSubMeshes())
				{
					MultiDrawIndirectCommand cmd{};
					cmd.Count = m.IndexCount;
					cmd.BaseInstance = i;
					cmd.BaseVertex = m.StartVertex;
					cmd.FirstIndex = m.StartIndex;
					cmd.InstanceCount = 1;
					commands.emplace_back(cmd);
				}
			}

			mMultiDrawIndirectBuffer = GPUBuffer::Create(sizeof(MultiDrawIndirectCommand) * commands.size(), EBufferType::IndirectBuffer);
			mMultiDrawIndirectBuffer->SetData(commands.data(), sizeof(MultiDrawIndirectCommand) * commands.size());
			mModelBuffer = GPUBuffer::Create(sizeof(FPerObjectData) * commands.size(), EBufferType::StorageBuffer);
		
		}

		{
			auto pipeline = PipelineRegistry::Get("Triangle");
			auto objectBindingGroup = pipeline->GetOrCreateBindingGroup(3);
			objectBindingGroup->SetBuffer(0, mModelBuffer);
			mMaterial = CreateRef<Material>();
			mMaterial->SetPipeline(pipeline);
			mMaterial->SetTexture("u_Texture", mTexture);
			mMaterial->Set("u_Color", glm::vec3(1, 1, 1));
		}

		{
			auto pipeline = PipelineRegistry::Get("Emissive");
			auto objectBindingGroup = pipeline->GetOrCreateBindingGroup(3);
			objectBindingGroup->SetBuffer(0, mModelBuffer);
			mEmissiveMaterial = CreateRef<EmissiveMaterial>();
			mEmissiveMaterial->SetPipeline(pipeline);
			mEmissiveMaterial->EmissionColor = FColor::Red;
		}

		{
			auto pipeline = PipelineRegistry::Get("Lambert");
			auto objectBindingGroup = pipeline->GetOrCreateBindingGroup(3);
			objectBindingGroup->SetBuffer(0, mModelBuffer);

			mLambertMaterial = CreateRef<LambertMaterial>();
			mLambertMaterial->SetPipeline(pipeline);
			mLambertMaterial->DiffuseColor = FColor::LightGray;
			mLambertMaterial->EmissionColor = {0.f, .0f, .0f};
			mLambertMaterial->SetTexture("DiffuseMap", mTexture);
		}

		
		
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

		
		auto &window = app.GetWindow();
		auto aspect = window.GetAspectRatio();

		mCamera = EditorCamera(75.f, aspect, 0.1f, 1000.f);
		mCamera.SetView(FTransform({5, 5, 5}));
		mCamera.Focus(FTransform({0, 0, 0}));

		FramebufferSpecification fb_specs;
		fb_specs.Size = window.GetSize();
		fb_specs.Attachments.attach(FTextureCreateInfo{
				.Format = EFormat::RGBA32F, .WrapMode = EWrapMode::CLAMP_TO_EDGE})
			.attach(FTextureCreateInfo{.Format = EFormat::DEPTH24_STENCIL8, .WrapMode = EWrapMode::CLAMP_TO_EDGE});

		mFramebuffer = Framebuffer::Create(fb_specs);
		
		
	
		auto &dbg = ImageDebugger::Get();
		dbg.Initialize({512, 512});

		auto &globalsResources = Renderer::Get().GetGlobalResources();  
		
		dbg.RegisterTexture("PreFilterEnv", globalsResources.Find("EnvironmentPreFilter")->TextureRef);
		dbg.RegisterTexture("EnvironmentCube", globalsResources.Find("EnvironmentCubeMap")->TextureRef);
		dbg.RegisterTexture("Irradiance", globalsResources.Find("EnvironmentIrradiance")->TextureRef);
		dbg.RegisterTexture("BRDFLUT", globalsResources.Find("EnvironmentBRDFLUT")->TextureRef);
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


		auto& scenepass = renderer.BeginPass("Scene", EPassType::OffScreen);
		scenepass.View = renderer.CreateView(mCamera.GetProjection(), mCamera.GetView());
		mFramebuffer->Bind();

	
		renderer.ClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		renderer.Clear();
		//renderer.SubmitCamera(mCamera.GetProjection(), mCamera.GetView());

		//main light source
		
		renderer.Light.Submit(mainLight);

		renderer.Light.Submit(plight0);

		renderer.Line.DrawGrid({});
		renderer.Line.DrawBox(glm::vec3{1.f}, glm::vec3{0.0f}, FColor::Blue, transform);
		renderer.Line.DrawLine({-1, 2, 0}, {1, 2, 0}, FColor::Green);
		

		FQuadParams params{.Size = {1, 1}, .Color = FColor::Red};
		renderer.Quad.DrawQuad(params, nullptr, FTransform({0, 0, 2}));

		params.Color = FColor::White;
		renderer.Quad.DrawQuad(params, mTexture, FTransform({0, 0, -2}));

		renderer.Quad.DrawCircle({.Radius = 1.f, .LineColor = FColor::Orange}, FTransform({2, 0, 0}));

		FTextParams tex_params{};
		renderer.Quad.DrawText(1.0f, "Cube", tex_params, FTransform({0, 2, 0}));

		renderer.Flush();

		std::vector<FPerObjectData> transforms(5);
		transforms[0].WorldMatrix = transform;
		transforms[1].WorldMatrix = FTransform({3, 4, 0});
		transforms[2].WorldMatrix = FTransform({4, 0, 0});
		transforms[3].WorldMatrix = FTransform({-4, 0, 0});
		transforms[4].WorldMatrix = FTransform({0, 0, 0});
		mModelBuffer->SetData(transforms.data(), sizeof(FPerObjectData) * transforms.size());

		const auto stride = sizeof(MultiDrawIndirectCommand);
		if (mMesh && mMaterial)
		{		
			mMaterial->Set("u_Time", Time::Raw());
			mMaterial->Submit();

			renderer.MultiDrawElementsIndirect(ETopologyMode::Triangles, mMultiDrawIndirectBuffer.get(), mMesh->GetVertexArray().get(), 2, stride );
		}

		if (mMesh && mEmissiveMaterial)
		{
			mEmissiveMaterial->Submit();

			renderer.MultiDrawElementsIndirect(ETopologyMode::Triangles, mMultiDrawIndirectBuffer.get(), mMesh->GetVertexArray().get(), 1, stride, 2u);
		}

		if (mSphere && mLambertMaterial)
		{
			mLambertMaterial->Submit();
			
			renderer.MultiDrawElementsIndirect(ETopologyMode::Triangles, mMultiDrawIndirectBuffer.get(), mSphere->GetVertexArray().get(), 2, stride, 3u);
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

		if (ImGui::Begin("Lights"))
		{
			ImGui::SeparatorText("MainLight");

			ImGui::DragFloat4("Color##Main", &mainLight.Color.x, .01f);
			ImGui::DragFloat4("Position##Main", &mainLight.Direction.x, .01f);

			ImGui::SeparatorText("PointLight0");
			ImGui::DragFloat4("Color##P0", &plight0.Color.x, .01f);
			ImGui::DragFloat4("Position##P0", &plight0.Position.x, .01f);
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