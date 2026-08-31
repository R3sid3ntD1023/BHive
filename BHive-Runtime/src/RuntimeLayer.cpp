#include "RuntimeLayer.h"

#include "core/Application.h"
#include "core/platform/Platform.h"
#include "core/Time.h"
#include "gfx/Texture.h"
#include "gui/Gui.h"
#include "importers/TextureImporter.h"
#include "gfx/material/Material.h"
#include "gfx/material/EmissiveMaterial.h"
#include "importers/MeshImporter.h"
#include "importers/MeshImportResolver.h"
#include "gfx/mesh/StaticMesh.h"
#include "gfx/renderers/Renderer.h"
#include "Inspectors/Inspect.h"
#include "gfx/material/LambertMaterial.h"
#include "gfx/debug/ImageDebugger.h"
#include "gfx/material/StandardMaterial.h"
#include "gfx/renderers/postprocess/AcesMaterial.h"
#include "gfx/renderers/postprocess/BloomMaterial.h"
#include "gfx/renderers/postprocess/ColorGradingMaterial.h"
#include "gfx/imgui/IImGuiProvider.h"
#include "core/layers/ImGuiLayer.h"
#include "gfx/factories/MeshFactory.h"
#include "gfx/factories/TextureFactory.h"

#define ENABLE_RENDERING 1

namespace BHive
{
	DirectionalLight mainLight{};
	PointLight pLight0{};
	SpotLight spLight0{};

	void RuntimeLayer::OnAttach(Application &app)
	{
		auto decodedSprite = TextureLoader::FromFile("C:/Users/dariu/Documents/BHive/projects/Mario/resources/sprites0.jpg");
		auto decodedEnviroment = TextureLoader::FromFile(ENGINE_PATH "/data/hdr/kloofendal_43d_clear_puresky_1k.hdr");

		mTexture = TextureFactory::Create2D(decodedSprite);

		// create mesh
		FMeshImportOptions import_options{.ImportMaterials = false};

		auto decoded = MeshImporter::Import("C:/Users/dariu/Documents/Cube.glb");
		std::vector<Ref<Asset>> additional_assets;
		MeshImportResolver resolver(import_options);
		mMesh = resolver.Resolve(decoded);

		mSphere = MeshFactory::CreateSphere(1.0f);
		mPlane = MeshFactory::CreatePlane(10.f, 10.f);
		mFont = FontFactory::Create(ENGINE_PATH "/data/fonts/Roboto/Roboto-Thin.ttf", 10.f);

		// create materials
		/*{
			auto emissiveMaterial = CreateRef<EmissiveMaterial>();
			emissiveMaterial->SetEmissionColor(FColor(1.0f, 0.0f, 0.0f, 10.0f));

			auto lambertMaterial0 = CreateRef<LambertMaterial>();
			lambertMaterial0->SetDiffuseColor(FColor::LightGray).SetEmissionColor(FColor::Black);
			lambertMaterial0->SetTexture("DiffuseMap", {mTexture});

			auto lambertMaterial1 = CreateRef<LambertMaterial>();
			lambertMaterial1->SetDiffuseColor(FColor::Orange).SetEmissionColor(FColor::Black);

			auto standardMaterial = CreateRef<StandardMaterial>();
			standardMaterial->SetAlbedo(FColor::White).SetEmission(FColor::Black).SetMetalness(1.0f).SetRoughness(0.5f);

			mMaterials[0].Add(lambertMaterial0);
			mMaterials[1].Add(lambertMaterial1);
			mMaterials[2].Add(emissiveMaterial);
			mMaterials[3].Add(standardMaterial);
		}*/

		auto &window = app.GetWindow();
		auto aspect = window.GetAspectRatio();

		mViewportSize = window.GetSize();

		mCamera = EditorCamera(75.f, aspect, 0.1f, 1000.f);
		mCamera.SetStartState({5, 5, 5}, 0, -45.f);

		auto &dbg = ImageDebugger::Get();
		dbg.Initialize({512, 512});

		mSceneRenderer = CreateRef<SceneRenderer>();
		mSceneRenderer->Init(mViewportSize);
		mSceneRenderer->SetEnvironmentTexture(TextureFactory::Create2D(decodedEnviroment));

		mSceneRenderer->AddPostProcessMaterial<BloomMaterial>();
		mSceneRenderer->AddPostProcessMaterial<AcesMaterial>();
		mSceneRenderer->AddPostProcessMaterial<ColorGradingMaterial>();

		mObjectTransforms[0] = FTransform({0, 1, 0});
		mObjectTransforms[1] = FTransform({3, 4, 0});
		mObjectTransforms[2] = FTransform({4, 1, 0});
		mObjectTransforms[3] = FTransform({-4, 1, 0});
		mObjectTransforms[4] = FTransform({0, 0, 0});

		auto &environment = mSceneRenderer->GetEnvironmentSystem();
		auto &maps = environment.GetCurrentMaps();
		auto brdfLUT = environment.GetBRDFLUT();
		dbg.RegisterTexture("PreFilterEnv", maps.PreFilter);
		dbg.RegisterTexture("EnvironmentCube", maps.Environment);
		dbg.RegisterTexture("Irradiance", maps.Irradiance);
		dbg.RegisterTexture("BRDFLUT", brdfLUT);

		mainLight.SetColor(FColor::White).SetDirection({-1, 0, 0}).SetIntensity(1.f);
		pLight0.SetColor(FColor::Orange).SetIntensity(3.f).SetPosition({4, 1, 0}).SetRadius(5.f);
		spLight0.SetColor(FColor::Red).SetIntensity(3.f).SetDirection({0, -1, 0}).SetPosition({}).SetRadius(5.f).SetInnerAngleDegrees(45.f).SetOuterAngleDegrees(75.f);

		mCameraController.SetCamera(&mCamera);
	}

	void RuntimeLayer::OnDetach()
	{
		ImageDebugger::Get().Shutdown();
	}

	void RuntimeLayer::OnUpdate(float time)
	{
		mObjectTransforms[0].AddRotation({0, time * 10.f, 0});

		if (mViewportActive)
		{
			mCameraController.Update(time);
		}
	}

	void RuntimeLayer::OnRender(Renderer &renderer)
	{
#if ENABLE_RENDERING

		auto size = mSceneRenderer->GetSize();
		if (mViewportSize != size && mViewportSize.x > 0 && mViewportSize.y > 0)
		{
			mSceneRenderer->Resize(mViewportSize);
			mCamera.Resize(mViewportSize.x, mViewportSize.y);
		}

		mSceneRenderer->Begin(&mCamera, mCamera.GetView());

		// Submit lights
		{

			mSceneRenderer->Submit(mainLight);
			mSceneRenderer->Submit(pLight0);
			mSceneRenderer->Submit(spLight0);
		}

		// submit meshes
		{
			std::vector<FMeshSubmissionRequest> infos{};

			// if (mMesh)
			// {
			// 	infos.emplace_back(mMesh, mMaterials[0], mObjectTransforms[0]);
			// 	infos.emplace_back(mMesh, mMaterials[1], mObjectTransforms[1]);
			// }

			infos.emplace_back(mSphere, mMaterials[2], mObjectTransforms[2]);
			infos.emplace_back(mSphere, mMaterials[3], mObjectTransforms[3]);
			infos.emplace_back(mPlane, mMaterials[3], mObjectTransforms[4]);

			for (auto &info : infos)
				mSceneRenderer->SubmitMesh(info);
		}

		// lines
		{
			FQuadParams params{.Size = {1, 1}, .Color = FColor::Red};
			FTextParams tex_params{};
			renderer.Line.DrawLine(glm::vec3{0.f, 0.f, 0.f}, mainLight.GetDirection(), mainLight.GetColor());
			renderer.Line.DrawSphere(pLight0.GetRadius(), 32, {}, pLight0.GetColor(), FTransform{pLight0.GetPosition()});
			renderer.Line.DrawSpotlightCone(spLight0.GetPosition(), spLight0.GetDirection(), spLight0.GetRadius(), spLight0.GetOuterAngleDegrees(), 32, spLight0.GetColor());
			renderer.Line.DrawGrid({});
			renderer.Line.DrawBox(glm::vec3{1.f}, glm::vec3{0.0f}, FColor::Blue, mObjectTransforms[0]);
			renderer.Line.DrawLine({-1, 2, 0}, {1, 2, 0}, FColor::Green);

			renderer.Quad.DrawQuad(params, {}, FTransform({0, 0, 2}));

			params.Color = FColor::White;
			renderer.Quad.DrawQuad(params, mTexture, FTransform({0, 0, -2}));

			renderer.Quad.DrawCircle({.Radius = 1.f, .LineColor = FColor::Orange}, FTransform({2, 0, 0}));
			renderer.Quad.DrawText(mFont, 1.0f, "Cube", tex_params, FTransform({0, 2, 0}));
		}

		mSceneRenderer->End();

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

			auto output = mSceneRenderer->GetOutput();
			if (output)
			{
				auto id = IImGuiTextureProvider::GetID(*output.As<Texture>());
				ImGui::Image(id, viewportSize);
			}
		}

		mViewportActive = ImGui::IsWindowHovered() || ImGui::IsWindowFocused();
		Application::Get().BlockImGuiEvents(!mViewportActive);

		ImGui::End();

		if (ImGui::Begin("Lights", 0, ImGuiWindowFlags_AlwaysHorizontalScrollbar))
		{
			Inspect::get().inspect("MainLight", mainLight);

			Inspect::get().inspect("PointLight0", pLight0);

			Inspect::get().inspect("SpotLight0", spLight0);

			auto &inspector = Inspect::get();

			{
				static BloomMaterial::FParams params{};
				ImGui::SeparatorText("BloomSettings");
				bool changed = inspector.inspect("Threshold", params.Threshold);
				changed |= inspector.inspect("FilterRadius", params.Radius);
				changed |= inspector.inspect("Strength", params.Strength);
				changed |= inspector.inspect("Exposure", params.Exposure);
				if (changed)
				{
					mSceneRenderer->GetPostProcessStack().Get<BloomMaterial>()->Params = params;
				}
			}

			{
				static ColorGradingMaterial::FParams params{};
				ImGui::SeparatorText("Color Grading");
				bool changed = inspector.inspect("Lift", params.Lift);
				changed |= inspector.inspect("Gamma", params.Gamma);
				changed |= inspector.inspect("Gain", params.Gain);
				changed |= inspector.inspect("Saturation", params.Saturation);
				if (changed)
				{
					mSceneRenderer->GetPostProcessStack().Get<ColorGradingMaterial>()->Params = params;
				}
			}
		}

		ImGui::End();

		if (ImGui::Begin("Window"))
		{
			auto id = IImGuiTextureProvider::GetID(*mTexture.As<Texture>());
			ImGui::Image(id, {200, 200}, {0, 1}, {1, 0});

			Inspect::get().inspect("Transform", mObjectTransforms[0]);

			if (ImGui::Button("Load Mesh"))
			{
				auto info = Platform::OpenFile("Mesh (*.glb;*.gltf)\0*.glb;*.gltf\0");
				if (info)
				{
					FMeshImportOptions import_options{};
					auto decoded = MeshImporter::Import(info);
					MeshImportResolver resolver(import_options);
					mMesh = resolver.Resolve(decoded);
				}
			}

			if (ImGui::Button("Load HDR"))
			{
				auto info = Platform::OpenFile("HDR (*.hdr;)\0*.hdr;\0");
				if (info)
				{
					auto decodedEnironment = TextureLoader::FromFile(info.Path);
					auto tex = TextureFactory::Create2D(decodedEnironment);

					mSceneRenderer->SetEnvironmentTexture(tex);

					auto &environment = mSceneRenderer->GetEnvironmentSystem();
					auto &maps = environment.GetCurrentMaps();
					auto &dbg = ImageDebugger::Get();

					auto env_prefilter = maps.PreFilter;
					auto env_cube = maps.Environment;
					auto env_irradiance = maps.Irradiance;

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
	}

	bool RuntimeLayer::OnWindowResize(WindowResizeEvent &e)
	{

		return false;
	}

} // namespace BHive