#include "RuntimeLayer.h"

#include "core/Application.h"
#include "core/platform/Platform.h"
#include "core/Time.h"
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
#include "Inspectors/Inspect.h"
#include "gfx/Framebuffer.h"
#include "gfx/material/LambertMaterial.h"
#include "gfx/debug/ImageDebugger.h"
#include "gfx/mesh/primitives/Sphere.h"
#include "gfx/mesh/primitives/Plane.h"
#include "gfx/material/StandardMaterial.h"
#include "gfx/renderers/postprocess/AcesMaterial.h"
#include "gfx/renderers/postprocess/BloomMaterial.h"
#include "gfx/renderers/postprocess/ColorGradingMaterial.h"
#include "gfx/imgui/IImGuiProvider.h"

#define ENABLE_RENDERING 1

namespace BHive
{
	FTransform transform{{6.f, 0.f, 0.f}};

	const uint32_t objectCount = 3;

	static DirectionalLight mainLight;
	static PointLight plight0;
	static SpotLight spLight0;

	void RuntimeLayer::OnAttach(Application &app)
	{
		mEnvironmentTex = TextureLoader::Import(ENGINE_PATH "/data/hdr/kloofendal_43d_clear_puresky_1k.hdr");
		Renderer::Get().SetEnvironmentTexture(mEnvironmentTex);

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

		// create model buffers
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

			mMultiDrawIndirectBuffer = GeneralBuffer::Create(sizeof(MultiDrawIndirectCommand) * commands.size(), EBufferType::IndirectBuffer);
			mMultiDrawIndirectBuffer->SetData(commands.data(), sizeof(MultiDrawIndirectCommand) * commands.size());
		}

		mEmissiveMaterial = CreateRef<EmissiveMaterial>();
		mEmissiveMaterial->SetEmissionColor(FColor(1.0f, 0.0f, 0.0f, 10.0f));

		mLambertMaterials[0] = CreateRef<LambertMaterial>();
		mLambertMaterials[0]->SetDiffuseColor(FColor::LightGray).SetEmissionColor(FColor::Black);
		mLambertMaterials[0]->SetTexture("DiffuseMap", {mTexture});

		mLambertMaterials[1] = CreateRef<LambertMaterial>();
		mLambertMaterials[1]->SetDiffuseColor(FColor::Orange).SetEmissionColor(FColor::Black);

		mStandardMaterial = CreateRef<StandardMaterial>();
		mStandardMaterial->SetAlbedo(FColor::White).SetEmission(FColor::Black).SetMetalness(1.0f).SetRoughness(0.5f);

		auto &window = app.GetWindow();
		auto aspect = window.GetAspectRatio();

		mViewportSize = window.GetSize();

		mCamera = EditorCamera(75.f, aspect, 0.1f, 1000.f);
		mCamera.SetView(FTransform({5, 5, 5}));
		mCamera.Focus(FTransform({0, 0, 0}));

		FramebufferSpecification fb_specs;
		fb_specs.Size = window.GetSize();
		fb_specs.Attachments.AddColorAttachment({FTextureCreateInfo{.Format = EFormat::RGBA32F, .WrapMode = EWrapMode::CLAMP_TO_EDGE}})
			.SetDepthAttachment({FTextureCreateInfo{.Format = EFormat::DEPTH24_STENCIL8, .WrapMode = EWrapMode::CLAMP_TO_EDGE}});

		fb_specs.DebugName = "Runtime";
		mFramebuffer = Framebuffer::Create(fb_specs);

		auto &globalsResources = Renderer::Get().GetGlobalResources();

		auto &dbg = ImageDebugger::Get();
		dbg.Initialize({512, 512});
		dbg.RegisterTexture("PreFilterEnv", globalsResources.Find("EnvironmentPreFilter")->TextureRef);
		dbg.RegisterTexture("EnvironmentCube", globalsResources.Find("EnvironmentCubeMap")->TextureRef);
		dbg.RegisterTexture("Irradiance", globalsResources.Find("EnvironmentIrradiance")->TextureRef);
		dbg.RegisterTexture("BRDFLUT", globalsResources.Find("EnvironmentBRDFLUT")->TextureRef);

		mainLight.SetColor(FColor::White).SetDirection({-1, 0, 0}).SetIntensity(1.f);
		plight0.SetColor(FColor::Orange).SetIntensity(3.f).SetPosition({4, 1, 0}).SetRadius(5.f);
		spLight0.SetColor(FColor::Red).SetIntensity(3.f).SetDirection({0, -1, 0}).SetPosition({}).SetRadius(5.f).SetInnerAngleDegrees(45.f).SetOuterAngleDegrees(75.f);

		// post process
		mPostProcessStack.Add<BloomMaterial>("Bloom");
		mPostProcessStack.Add<AcesMaterial>("Aces");
		mPostProcessStack.Add<ColorGradingMaterial>("ColorGrading");
		mPostProcessStack.Resize(window.GetSize(), mPostProcessAllocator);
	}

	void RuntimeLayer::OnDetach()
	{
	}

	void RuntimeLayer::OnUpdate(float time)
	{
		// LOG_INFO("Frame time: {} ms", Time::DeltaTime() * 1000.0);

		transform.AddRotation({0, time * 10.f, 0});

		if (mViewportActive)
		{
			mCamera.ProcessInput();
		}
	}

	void RuntimeLayer::OnRender(Renderer &renderer)
	{
#if ENABLE_RENDERING

		if (mFramebuffer)
		{
			auto fbSize = mFramebuffer->GetSize();
			if (mViewportSize != fbSize && mViewportSize.x > 0 && mViewportSize.y > 0)
			{
				mFramebuffer->Resize(mViewportSize);
				mPostProcessStack.Resize(mViewportSize, mPostProcessAllocator);
				mCamera.Resize(mViewportSize.x, mViewportSize.y);

				IImGuiTextureProvider::Invalidate(*mFinalSceneColor);
			}
		}

		// Submit lights
		{
			renderer.Light.Submit(mainLight);
			renderer.Light.Submit(plight0);
			renderer.Light.Submit(spLight0);
		}

		// Update model matrices
		{
			std::vector<FPerObjectData> transforms(6);
			transforms[0].WorldMatrix = transform;
			transforms[1].WorldMatrix = FTransform({3, 4, 0});
			transforms[2].WorldMatrix = FTransform({4, 1, 0});
			transforms[3].WorldMatrix = FTransform({-4, 1, 0});
			transforms[4].WorldMatrix = FTransform({0, 1, 0});
			transforms[5].WorldMatrix = FTransform({0, 0, 0});
			renderer.SetPerObjectData(transforms.data(), transforms.size());
		}

		auto &globalsResources = Renderer::Get().GetGlobalResources();
		FView view = renderer.CreateView(mCamera.GetProjection(), mCamera.GetView());

		{
			FPassState state{};
			state.Color = {EAttachmentLoadState::Clear, EAttachmentStoreState::Store, {0.1f, 0.1f, 0.1f, 1.0f}};
			state.Depth = {EAttachmentLoadState::Clear, EAttachmentStoreState::Store};
			auto &scenePass = renderer.BeginPass("Scene", EPassType::OffScreen, state);

			scenePass.BeginPhase();
			scenePass.Push(view);
			scenePass.Push(mFramebuffer);
			scenePass.Push(globalsResources.Find("EnvironmentPreFilter")->TextureRef, EImageAccess::ColorRead);
			scenePass.Push(globalsResources.Find("EnvironmentCubeMap")->TextureRef, EImageAccess::ColorRead);
			scenePass.Push(globalsResources.Find("EnvironmentIrradiance")->TextureRef, EImageAccess::ColorRead);
			scenePass.Push(globalsResources.Find("EnvironmentBRDFLUT")->TextureRef, EImageAccess::ColorRead);
			scenePass.Push(globalsResources.Find("Camera")->BufferRef.get(), EBufferAccess::UniformRead);

			scenePass.Emplace<CmdBindPipeline>()(PipelineRegistry::Get("MESH_OPAQUE"));

			const auto stride = sizeof(MultiDrawIndirectCommand);
			if (mMesh)
			{
				mMesh->GetVertexArray()->DeclareAccess(scenePass, EBufferAccess::IndirectRead, EBufferAccess::IndirectRead);

				scenePass.Emplace<CmdBindMaterial>()(mLambertMaterials[0].get());
				scenePass.Emplace<CmdMultiDrawIndexedIndirect>()(ETopologyMode::Triangles, mMultiDrawIndirectBuffer.get(), mMesh->GetVertexArray().get(), 2u, stride);

				scenePass.Emplace<CmdBindMaterial>()(mLambertMaterials[1].get());
				scenePass.Emplace<CmdMultiDrawIndexedIndirect>()(ETopologyMode::Triangles, mMultiDrawIndirectBuffer.get(), mMesh->GetVertexArray().get(), 1u, stride, 2u);
			}

			mSphere->GetVertexArray()->DeclareAccess(scenePass, EBufferAccess::IndirectRead, EBufferAccess::IndirectRead);
			mPlane->GetVertexArray()->DeclareAccess(scenePass, EBufferAccess::IndirectRead, EBufferAccess::IndirectRead);

			scenePass.Emplace<CmdBindMaterial>()(mEmissiveMaterial.get());
			scenePass.Emplace<CmdMultiDrawIndexedIndirect>()(ETopologyMode::Triangles, mMultiDrawIndirectBuffer.get(), mSphere->GetVertexArray().get(), 1u, stride, 3u);

			scenePass.Emplace<CmdBindMaterial>()(mStandardMaterial.get());
			scenePass.Emplace<CmdMultiDrawIndexedIndirect>()(ETopologyMode::Triangles, mMultiDrawIndirectBuffer.get(), mSphere->GetVertexArray().get(), 1u, stride, 4u);
			scenePass.Emplace<CmdMultiDrawIndexedIndirect>()(ETopologyMode::Triangles, mMultiDrawIndirectBuffer.get(), mPlane->GetVertexArray().get(), 1u, stride, 5u);

			{
				FQuadParams params{.Size = {1, 1}, .Color = FColor::Red};
				FTextParams tex_params{};

				renderer.Line.DrawLine(glm::vec3{0.f, 0.f, 0.f}, mainLight.GetDirection(), mainLight.GetColor());
				renderer.Line.DrawSphere(plight0.GetRadius(), 32, {}, plight0.GetColor(), FTransform{plight0.GetPosition()});
				renderer.Line.DrawSpotlightCone(spLight0.GetPosition(), spLight0.GetDirection(), spLight0.GetRadius(), spLight0.GetOuterAngleDegrees(), 32, spLight0.GetColor());
				renderer.Line.DrawGrid({});
				renderer.Line.DrawBox(glm::vec3{1.f}, glm::vec3{0.0f}, FColor::Blue, transform);
				renderer.Line.DrawLine({-1, 2, 0}, {1, 2, 0}, FColor::Green);

				renderer.Quad.DrawQuad(params, nullptr, FTransform({0, 0, 2}));

				params.Color = FColor::White;
				renderer.Quad.DrawQuad(params, mTexture, FTransform({0, 0, -2}));

				renderer.Quad.DrawCircle({.Radius = 1.f, .LineColor = FColor::Orange}, FTransform({2, 0, 0}));
				renderer.Quad.DrawText(1.0f, "Cube", tex_params, FTransform({0, 2, 0}));
				renderer.Flush();
			}

			scenePass.EndPhase();

			renderer.EndPass();
		}

		{
			FPassState debugState{};
			debugState.Color = {EAttachmentLoadState::Load, EAttachmentStoreState::Store, {0.1f, 0.1f, 0.1f, 1.0f}};
			debugState.Depth = {EAttachmentLoadState::Load, EAttachmentStoreState::Store};

			auto &debugPass = renderer.BeginPass("DebugPass", EPassType::OffScreen, debugState);

			debugPass.BeginPhase("Draw Debug Objects", EPhaseType::Graphics);
			debugPass.Push(globalsResources.Find("Camera")->BufferRef.get(), EBufferAccess::UniformRead);
			debugPass.Push(view);
			debugPass.Push(mFramebuffer);

			{
				FQuadParams params{.Size = {1, 1}, .Color = FColor::Red};
				FTextParams tex_params{};

				renderer.Line.DrawLine(glm::vec3{0.f, 0.f, 0.f}, mainLight.GetDirection(), mainLight.GetColor());
				renderer.Line.DrawSphere(plight0.GetRadius(), 32, {}, plight0.GetColor(), FTransform{plight0.GetPosition()});
				renderer.Line.DrawSpotlightCone(spLight0.GetPosition(), spLight0.GetDirection(), spLight0.GetRadius(), spLight0.GetOuterAngleDegrees(), 32, spLight0.GetColor());
				renderer.Line.DrawGrid({});
				renderer.Line.DrawBox(glm::vec3{1.f}, glm::vec3{0.0f}, FColor::Blue, transform);
				renderer.Line.DrawLine({-1, 2, 0}, {1, 2, 0}, FColor::Green);

				renderer.Quad.DrawQuad(params, nullptr, FTransform({0, 0, 2}));

				params.Color = FColor::White;
				renderer.Quad.DrawQuad(params, mTexture, FTransform({0, 0, -2}));

				renderer.Quad.DrawCircle({.Radius = 1.f, .LineColor = FColor::Orange}, FTransform({2, 0, 0}));
				renderer.Quad.DrawText(1.0f, "Cube", tex_params, FTransform({0, 2, 0}));
			}

			renderer.Flush();
			debugPass.EndPhase();

			debugPass.BeginPhase("Transition to Read", EPhaseType::Transfer);
			debugPass.Push(mFramebuffer->GetColorAttachment(), EImageAccess::ColorRead);
			debugPass.EndPhase();

			renderer.EndPass();
		}

		mFinalSceneColor = mFramebuffer->GetColorAttachment();
		mFinalSceneColor = mPostProcessStack.Build(renderer.GetActiveGraph(), mPostProcessAllocator, mFinalSceneColor);
#endif
		ImageDebugger::Get().OnRender(renderer);
	}

	void RuntimeLayer::OnGuiRender()
	{

		ImageDebugger::Get().OnGuiRender();

		if (ImGui::Begin("Scene"))
		{
			auto viewportSize = ImGui::GetContentRegionAvail();
			mViewportSize = {uint32_t(glm::round(viewportSize.x)), uint32_t(glm::round(viewportSize.y))};

			if (mFinalSceneColor)
			{
				auto id = IImGuiTextureProvider::GetID(*mFinalSceneColor);
				ImGui::Image(id, viewportSize);
			}
		}

		mViewportActive = ImGui::IsWindowHovered() && ImGui::IsWindowFocused();

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
					mPostProcessStack.Get<BloomMaterial>("Bloom")->Params = params;
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
					mPostProcessStack.Get<ColorGradingMaterial>("ColorGrading")->Params = params;
				}
			}
		}

		ImGui::End();

		if (ImGui::Begin("Window"))
		{
			auto id = IImGuiTextureProvider::GetID(*mTexture);
			ImGui::Image(id, {200, 200}, {0, 1}, {1, 0});

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

					if (env_prefilter)
					{
						dbg.RegisterTexture("PreFilterEnv", env_prefilter);
					}

					if (env_irradiance)
					{
						dbg.RegisterTexture("Irradiance", env_irradiance);
					}

					if (env_cube)
						dbg.RegisterTexture("EnvironmentCube", env_cube);
				}
			}
		}

		ImGui::End();
	}

	void RuntimeLayer::OnEvent(Event &e)
	{
		if (mViewportActive)
		{
			mCamera.OnEvent(e);
		}

		/*EventDispatcher dispatcher(e);
		dispatcher.Dispatch(this, &RuntimeLayer::OnWindowResize);*/
	}

	bool RuntimeLayer::OnWindowResize(WindowResizeEvent &e)
	{

		// mCamera.Resize(e.x, e.y);

		return false;
	}

} // namespace BHive