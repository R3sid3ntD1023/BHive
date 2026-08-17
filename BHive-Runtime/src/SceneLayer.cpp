#include "SceneLayer.h"

#include "core/Application.h"
#include "gui/Gui.h"
#include "gfx/imgui/IImGuiProvider.h"
#include "gfx/renderers/SceneRenderer.h"
#include "gfx/Framebuffer.h"
#include "gfx/mesh/primitives/Sphere.h"
#include "gfx/mesh/primitives/Plane.h"
#include "gfx/material/StandardMaterial.h"
#include "gfx/material/LambertMaterial.h"
#include "importers/TextureImporter.h"
#include "importers/MeshImporter.h"
#include "importers/MeshImportResolver.h"
#include "gfx/renderers/postprocess/BloomMaterial.h"
#include "gfx/renderers/postprocess/AcesMaterial.h"
#include "gfx/renderers/postprocess/ColorGradingMaterial.h"
#include "Inspectors/Inspect.h"
#include "core/WindowInput.h"
#include "core/layers/ImGuiLayer.h"

namespace BHive
{
	FTransform sphereTransform{{5, -1.f, 2}};

	void SceneLayer::OnAttach(Application &app)
	{
		auto &window = app.GetWindow();
		auto aspect = window.GetAspectRatio();

		mViewportSize = window.GetSize();

		mCamera = EditorCamera(75.f, aspect, 0.1f, 1000.f);
		mCamera.SetPosition({5, 5, 5});
		mCamera.Focus({0, 0, 0});
		mCamera.SetPitch(-45.f);

		mSceneRenderer = CreateRef<SceneRenderer>();
		mSceneRenderer->Init(mViewportSize);
		mSceneRenderer->SetEnvironmentTexture(TextureLoader::Import(ENGINE_PATH "/data/hdr/kloofendal_43d_clear_puresky_1k.hdr"));

		mSceneRenderer->AddPostProcessMaterial<BloomMaterial>();
		mSceneRenderer->AddPostProcessMaterial<AcesMaterial>();
		mSceneRenderer->AddPostProcessMaterial<ColorGradingMaterial>();

		/*FMeshImportData import_data{};
		FMeshImportOptions import_options{.ImportMaterials = false};

		if (MeshImporter::Import("C:/Users/dariu/Documents/Cube.glb", import_data))
		{
			std::vector<Ref<Asset>> additional_assets;
			MeshImportResolver resolver(import_data, import_options, additional_assets);
			mMesh = Cast<StaticMesh>(resolver.Resolve());
		}*/

		mMesh = CreateRef<PSphere>(1.0f);
		mPlane = CreateRef<PPlane>(10.f, 10.0f);

		{

			{
				auto texture = TextureLoader::Import("C:/Users/dariu/Documents/BHive/projects/Mario/resources/sprites0.jpg", {});
				auto material = CreateRef<LambertMaterial>();
				material->SetDiffuseColor(FColor::DarkGray).SetEmissionColor(FColor::Black);
				material->SetTexture("DiffuseMap", {texture});
				mMaterialTables[0].add_material(material);
			}

			{
				auto texture = TextureLoader::Import("C:/Users/dariu/Documents/BHive/projects/Mario/resources/textures/Mario.png", {});
				auto material = CreateRef<LambertMaterial>();
				material->SetDiffuseColor({.2f, .2f, .2f, 1.0f});
				material->SetTexture("DiffuseMap", {texture});
				material->SetSurfaceType(Material::ESurfaceType::Transparent);
				mMaterialTables[1].add_material(material);
			}
		}

		{
			auto material = CreateRef<StandardMaterial>();
			material->SetAlbedo(FColor::Gray).SetEmission(FColor::Black).SetMetalness(1.0f).SetRoughness(0.1f);
			mMaterialTables[2].add_material(material);
		}

		mCameraController.SetCamera(&mCamera);
	}

	void SceneLayer::OnDetach()
	{
	}

	void SceneLayer::OnUpdate(float time)
	{
		sphereTransform.AddRotation({0, .1f, 0});

		if (mViewportActive)
			mCameraController.Update(time);
	}

	void SceneLayer::OnRender(Renderer &renderer)
	{
		auto size = mSceneRenderer->GetSize();
		if (mViewportSize != size && mViewportSize.x > 0 && mViewportSize.y > 0)
		{
			mSceneRenderer->Resize(mViewportSize);
			mCamera.Resize(mViewportSize.x, mViewportSize.y);

			IImGuiTextureProvider::Invalidate(*mSceneRenderer->GetOutput());
		}

		mSceneRenderer->Begin(&mCamera, mCamera.GetView());

		FMeshInfo info{};
		info.Mesh = mMesh;
		info.Materials = mMaterialTables[0];
		info.Transform = sphereTransform;
		mSceneRenderer->Submit(info);

		info.Materials = mMaterialTables[2];
		info.Transform = FTransform{{0, 1.f, -2}};

		mSceneRenderer->Submit(info);

		info.Materials = mMaterialTables[1];
		info.Transform = FTransform{{0, 0, 0}};
		info.Mesh = mPlane;

		mSceneRenderer->Submit(info);

		DirectionalLight main{};
		main.SetColor(FColor::White).SetIntensity(1.0f).SetDirection({0.f, -1.0f, 0.5f});
		mSceneRenderer->Submit(main);

		PointLight light{};
		light.SetColor(FColor::White).SetIntensity(1.0f).SetRadius(10.f).SetPosition({0, 2, 0});
		renderer.Line.DrawSphere(light.GetRadius(), 20, {}, light.GetColor(), light.GetPosition());
		renderer.Line.DrawGrid({});

		mSceneRenderer->Submit(light);
		mSceneRenderer->End();
	}

	void SceneLayer::OnGuiRender()
	{
		if (ImGui::Begin("SceneRenderer"))
		{
			auto viewportSize = ImGui::GetContentRegionAvail();
			mViewportSize = {uint32_t(glm::round(viewportSize.x)), uint32_t(glm::round(viewportSize.y))};

			auto output = mSceneRenderer->GetOutput();
			if (output)
			{
				auto id = IImGuiTextureProvider::GetID(*output);
				ImGui::Image(id, viewportSize);
			}
		}

		mViewportActive = ImGui::IsWindowHovered() || ImGui::IsWindowFocused();
		Application::Get().GetImGuiLayer()->BlockEvents(!mViewportActive);

		ImGui::End();

		if (ImGui::Begin("Post Process"))
		{
			auto &inspector = Inspect::get();
			auto &stack = mSceneRenderer->GetPostProcessStack();

			auto bloom = stack.Get<BloomMaterial>();
			auto colorGrading = stack.Get<ColorGradingMaterial>();

			if (bloom)
				inspector.inspect("Bloom", bloom, bloom->Params);
			if (colorGrading)
				inspector.inspect("Color Grading", colorGrading, colorGrading->Params);
		}

		ImGui::End();
	}

} // namespace BHive