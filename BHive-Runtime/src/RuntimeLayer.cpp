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
#include "gfx/mesh/primitives/Plane.h"
#include "gfx/material/StandardMaterial.h"
#include "gfx/renderers/postprocess/AcesMaterial.h"
#include "gfx/renderers/postprocess/BloomMaterial.h"
#include "gfx/renderers/postprocess/ColorGradingMaterial.h"

namespace BHive
{
	FTransform transform{{6.f, 0.f, 0.f}};

	struct FPerObjectData
	{
		glm::mat4 WorldMatrix = {1.0f};
	};

	const uint32_t objectCount = 3;

	static DirectionalLight mainLight;
	static PointLight plight0;
	static SpotLight spLight0;

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

		{
			auto shader = ShaderManager::Get("BDRFMaterial.glsl");
			auto state = Pipeline::GetDefaultGraphicsPipelineState();
			state.ShaderProgram = shader;
			state.ColorAttachmentFormats = {EFormat::RGBA32F};
			PipelineRegistry::Register("Standard", state);
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

		mPlane = CreateRef<PPlane>(10.f, 10.f);
		
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

			for (auto &m : mPlane->GetSubMeshes())
			{
				MultiDrawIndirectCommand cmd{};
				cmd.Count = m.IndexCount;
				cmd.BaseInstance = i;
				cmd.BaseVertex = m.StartVertex;
				cmd.FirstIndex = m.StartIndex;
				cmd.InstanceCount = 1;
				commands.emplace_back(cmd);
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
			mEmissiveMaterial->EmissionColor.a = 10.0f;
		}

		{
			auto pipeline = PipelineRegistry::Get("Lambert");
			auto objectBindingGroup = pipeline->GetOrCreateBindingGroup(3);
			objectBindingGroup->SetBuffer(0, mModelBuffer);

			mLambertMaterial = CreateRef<LambertMaterial>();
			mLambertMaterial->SetPipeline(pipeline);
			mLambertMaterial->DiffuseColor = FColor::LightGray;
			mLambertMaterial->EmissionColor = {0.f, .0f, .0f};
			//mLambertMaterial->SetTexture("DiffuseMap", mTexture);
		}

		{
			auto pipeline = PipelineRegistry::Get("Standard");
			auto objectBindingGroup = pipeline->GetOrCreateBindingGroup(3);
			objectBindingGroup->SetBuffer(0, mModelBuffer);

			mStandardMaterial = CreateRef<StandardMaterial>();
			mStandardMaterial->SetPipeline(pipeline);
			mStandardMaterial->Albedo = FColor::White;
			mStandardMaterial->Emission = {0.f, .0f, .0f};
			mStandardMaterial->Metallic = 1.0f;
			mStandardMaterial->Roughness = 0.5f;
			// mLambertMaterial->SetTexture("DiffuseMap", mTexture);
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

		mainLight.SetColor(FColor::White).SetDirection({-1, 0, 0}).SetIntensity(1.f);
		plight0.SetColor(FColor::Orange).SetIntensity(3.f).SetPosition({4, 1, 0}).SetRadius(5.f);
		spLight0.SetColor(FColor::Red).SetIntensity(3.f).SetDirection({0, -1, 0}).SetPosition({}).SetRadius(5.f).SetInnerAngleDegrees(45.f).SetOuterAngleDegrees(75.f);

		//post process

		auto aces = CreateRef<AcesMaterial>();
		aces->CreateResizableObjects(window.GetSize());

		mBloomMaterial = CreateRef<BloomMaterial>();
		mBloomMaterial->CreateResizableObjects(window.GetSize());

		mColorGrading = CreateRef<ColorGradingMaterial>();
		mColorGrading->CreateResizableObjects(window.GetSize());

		mPostProcessStack.Materials.push_back(mBloomMaterial);
		mPostProcessStack.Materials.push_back(aces);
		mPostProcessStack.Materials.push_back(mColorGrading);
		
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
		renderer.Line.DrawLine(glm::vec3{0.f, 0.f, 0.f}, mainLight.GetDirection(), mainLight.GetColor());

		renderer.Light.Submit(plight0);
		renderer.Line.DrawSphere(plight0.GetRadius(), 32, {}, plight0.GetColor(), FTransform{plight0.GetPosition()});

		renderer.Light.Submit(spLight0);
		renderer.Line.DrawSpotlightCone(spLight0.GetPosition(), spLight0.GetDirection(), spLight0.GetRadius(), spLight0.GetOuterAngleDegrees(), 32, spLight0.GetColor());

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

		std::vector<FPerObjectData> transforms(6);
		transforms[0].WorldMatrix = transform;
		transforms[1].WorldMatrix = FTransform({3, 4, 0});
		transforms[2].WorldMatrix = FTransform({4, 1, 0});
		transforms[3].WorldMatrix = FTransform({-4, 1, 0});
		transforms[4].WorldMatrix = FTransform({0, 1, 0});
		transforms[5].WorldMatrix = FTransform({0, 0, 0});
		mModelBuffer->SetData(transforms.data(), sizeof(FPerObjectData) * transforms.size());

		const auto stride = sizeof(MultiDrawIndirectCommand);
		if (mMesh && mMaterial)
		{		
			mMaterial->Set("u_Time", Time::Raw());
			mMaterial->Submit();

			renderer.MultiDrawElementsIndirect(ETopologyMode::Triangles, mMultiDrawIndirectBuffer.get(), mMesh->GetVertexArray().get(), 2, stride );
		}

		if (mMesh && mLambertMaterial)
		{
			mLambertMaterial->Submit();

			renderer.MultiDrawElementsIndirect(ETopologyMode::Triangles, mMultiDrawIndirectBuffer.get(), mMesh->GetVertexArray().get(), 1, stride, 2u);
		}

		mEmissiveMaterial->Submit();

		renderer.MultiDrawElementsIndirect(ETopologyMode::Triangles, mMultiDrawIndirectBuffer.get(), mSphere->GetVertexArray().get(), 1, stride, 3u);

		mStandardMaterial->Submit();

		renderer.MultiDrawElementsIndirect(ETopologyMode::Triangles, mMultiDrawIndirectBuffer.get(), mSphere->GetVertexArray().get(), 1, stride, 4u);
		renderer.MultiDrawElementsIndirect(ETopologyMode::Triangles, mMultiDrawIndirectBuffer.get(), mPlane->GetVertexArray().get(), 1, stride, 5u);

		mFramebuffer->UnBind();

		renderer.EndPass();

		auto input = mFramebuffer->GetColorAttachment();
		mFinalSceneColor = mPostProcessStack.Build(renderer.GetActiveGraph(), input);

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
				glm::uvec2 size = {uint32_t(glm::round(viewportSize.x)), uint32_t(glm::round(viewportSize.y))};

				if (size != fbSize && size.x > 0 && size.y > 0)
				{
					mFramebuffer->Resize(size);
					mPostProcessStack.Resize(size);
					mCamera.Resize(size.x, size.y);
				}
				auto texture_id = ImGuiLayer::GetTextureID(*mFinalSceneColor);
				ImGui::Image(texture_id, viewportSize);
			}
		}

		ImGui::End();

		if (ImGui::Begin("Lights", 0, ImGuiWindowFlags_AlwaysHorizontalScrollbar))
		{
			Inspect::get().inspect("MainLight", mainLight);

			Inspect::get().inspect("PointLight0", plight0);

			Inspect::get().inspect("SpotLight0", spLight0);

			
			auto &inspector = Inspect::get();		

			{
				static BloomMaterial::FBloomParams params{};
				ImGui::SeparatorText("BloomSettings");
				bool changed = inspector.inspect("Threshold", params.Threshold);
				changed |= inspector.inspect("FilterRadius", params.Radius);
				changed |= inspector.inspect("Strength", params.Strength);
				changed |= inspector.inspect("Exposure", params.Exposure);
				if (changed)
				{
					mBloomMaterial->Params = params;
				}
			}
		
			
			{
				static ColorGradingMaterial::FColorGrading params{};
				ImGui::SeparatorText("Color Grading");
				bool changed = inspector.inspect("Lift", params.Lift);
				changed |= inspector.inspect("Gamma", params.Gamma);
				changed |= inspector.inspect("Gain", params.Gain);
				changed |= inspector.inspect("Saturation", params.Saturation);
				if (changed)
				{
					mColorGrading->Params = params;
				}
			}
		}

		ImGui::End();

		if (ImGui::Begin("Window"))
		{
			auto texture_id = ImGuiLayer::GetTextureID(*mTexture);
			ImGui::Image(texture_id, {200, 200}, {0, 1}, {1, 0});

			Inspect::get().inspect("Transform", transform);

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

					auto &globalsResources = Renderer::Get().GetGlobalResources();
					auto &dbg = ImageDebugger::Get();

					auto env_prefilter = globalsResources.Find("EnvironmentPreFilter")->TextureRef;
					auto env_cube = globalsResources.Find("EnvironmentCubeMap")->TextureRef;
					auto env_irradiance = globalsResources.Find("EnvironmentIrradiance")->TextureRef;
					auto pipeline = PipelineRegistry::Get("Standard");
					auto globalBindings = pipeline->GetOrCreateBindingGroup(0);

					if (env_prefilter)
					{
						dbg.RegisterTexture("PreFilterEnv", env_prefilter);
						globalBindings->SetTexture(3, env_prefilter);
					}
					
					if (env_irradiance)
					{
						dbg.RegisterTexture("Irradiance", env_irradiance);
						globalBindings->SetTexture(4, env_irradiance);
					}

					if (env_cube)
						dbg.RegisterTexture("EnvironmentCube", env_cube);
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