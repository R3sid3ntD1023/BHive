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

namespace BHive
{

	void SceneLayer::OnAttach(Application &app)
	{
		auto &window = app.GetWindow();
		auto aspect = window.GetAspectRatio();

		mViewportSize = window.GetSize();

		mCamera = EditorCamera(75.f, aspect, 0.1f, 1000.f);
		mCamera.SetView(FTransform({5, 5, 5}));
		mCamera.Focus(FTransform({0, 0, 0}));

		auto environmentTex = TextureLoader::Import(ENGINE_PATH "/data/hdr/kloofendal_43d_clear_puresky_1k.hdr");
		Renderer::Get().SetEnvironmentTexture(environmentTex);

		mSceneRenderer = CreateRef<SceneRenderer>();
		mSceneRenderer->Init(mViewportSize);

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
			auto material = CreateRef<LambertMaterial>();
			material->SetDiffuseColor(FColor::DarkGray).SetEmissionColor(FColor::Black);
			mMaterialTables[0].add_material(material);
		}

		{
			auto material = CreateRef<StandardMaterial>();
			material->SetAlbedo(FColor::Orange).SetEmission(FColor::Black).SetMetalness(1.0f).SetRoughness(0.4f);
			mMaterialTables[1].add_material(material);
		}
	}

	void SceneLayer::OnDetach()
	{
	}

	void SceneLayer::OnUpdate(float time)
	{
		if (mViewportActive)
		{
			mCamera.ProcessInput();
		}
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
		info.Transform = FTransform{{0, 1.f, 2}};

		mSceneRenderer->Submit(info);

		info.Materials = mMaterialTables[1];
		info.Transform = FTransform{{0, 1.f, -2}};

		mSceneRenderer->Submit(info);

		info.Materials = mMaterialTables[0];
		info.Transform = FTransform{{0, 0, 0}};
		info.Mesh = mPlane;

		mSceneRenderer->Submit(info);

		PointLight light{};
		light.SetColor(FColor::White).SetIntensity(10.0f).SetRadius(100.f).SetPosition({0, 2, 0});
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

		mViewportActive = ImGui::IsWindowHovered() && ImGui::IsWindowFocused();

		ImGui::End();
	}

	void SceneLayer::OnEvent(Event &e)
	{
		if (mViewportActive)
		{
			mCamera.OnEvent(e);
		}

		/*EventDispatcher dispatcher(e);
		dispatcher.Dispatch(this, &SceneLayer::OnWindowResize);*/
	}

	bool SceneLayer::OnWindowResize(WindowResizeEvent &e)
	{
		// mCamera.Resize(e.x, e.y);

		return false;
	}

} // namespace BHive