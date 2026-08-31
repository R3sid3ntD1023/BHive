#include "SceneLayer.h"

#include "core/Application.h"
#include "gui/Gui.h"
#include "gfx/imgui/IImGuiProvider.h"
#include "gfx/renderers/SceneRenderer.h"
#include "gfx/Framebuffer.h"
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
#include "core/platform/Platform.h"
#include "gfx/factories/MaterialFactory.h"
#include "gfx/factories/MeshFactory.h"
#include "gfx/factories/TextureFactory.h"

namespace BHive
{
	FTransform sphereTransform{{5, -1.f, 2}};
	std::vector<FTransform> transforms;
	ContextHandle sSphereHandle;
	ContextHandle sPlaneHandle;

	void SceneLayer::OnAttach(Application &app)
	{
		auto decodeEnvironment = TextureLoader::FromFile(ENGINE_PATH "/data/hdr/kloofendal_43d_clear_puresky_1k.hdr");
		auto decodedSprite = TextureLoader::FromFile("C:/Users/dariu/Documents/BHive/projects/Mario/resources/sprites0.jpg");
		auto decodedMario = TextureLoader::FromFile("C:/Users/dariu/Documents/BHive/projects/Mario/resources/textures/Mario.png");

		auto &window = app.GetWindow();
		auto aspect = window.GetAspectRatio();

		mViewportSize = window.GetSize();

		mCameras[0] = EditorCamera(75.f, aspect, 0.1f, 100.f);
		mCameras[0].SetStartState({0.f, 0.f, 0.f}, -90.0f, 0.0f);

		mCameras[1] = EditorCamera(75.f, aspect, 0.1f, 500.f);
		mCameras[1].SetStartState({0.f, 10.f, 10.f}, -90.0f, -45.0f);

		mSceneRenderer = CreateRef<SceneRenderer>();
		mSceneRenderer->Init(mViewportSize);
		mSceneRenderer->SetEnvironmentTexture(TextureFactory::Create2D(decodeEnvironment));

		// mSceneRenderer->AddPostProcessMaterial<BloomMaterial>();
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

		auto mesh = MeshFactory::CreateSphere(1.0f, 32u, 32u);
		auto plane = MeshFactory::CreatePlane(10.f, 10.f);

		{

			mMaterialTables[0].Add(MaterialFactory::CreateLambert());
			mMaterialTables[1].Add(MaterialFactory::CreateLambert());
			mMaterialTables[2].Add(MaterialFactory::CreateStandard());

			{

				auto texture = TextureFactory::Create2D(decodedSprite);
				auto material = mMaterialTables[0][0].As<LambertMaterial>();
				material->SetDiffuseColor(FColor::DarkGray).SetEmissionColor(FColor::Black);
				material->SetTexture("DiffuseMap", {texture});
			}

			{
				auto texture = TextureFactory::Create2D(decodedMario);
				auto material = mMaterialTables[1][0].As<LambertMaterial>();
				material->SetDiffuseColor({.2f, .2f, .2f, 1.0f});
				material->SetTexture("DiffuseMap", {texture});
				material->SetSurfaceType(Material::ESurfaceType::Transparent);
			}

			{
				auto material = mMaterialTables[2][0].As<StandardMaterial>();
				material->SetAlbedo({1.f, 0.5f, 0.f, 1.0f}).SetEmission(FColor::Black).SetMetalness(0.0f).SetRoughness(0.5f);
			}
		}

		mCameraController.SetCamera(&mCameras[0]);

		for (int32_t i = -1; i <= 1; i++)
		{
			for (int32_t j = -1; j <= 1; j++)
			{
				FMeshSubmissionRequest request{};
				request.Mesh = mesh;
				request.Materials = mMaterialTables[2];
				request.Transform = transforms.emplace_back(FTransform{{i * 3.0f, 0.0f, j * 3.0f}});
				mSceneRenderer->SubmitMesh(request, sSphereHandle);
			}
		}

		// FMeshSubmissionRequest request{};
		// request.Mesh = mMesh;
		// request.Materials = mMaterialTables[2];
		// request.Transform = FTransform{{0.f, 0.0f, 0.f}};
		// mSceneRenderer->SubmitMesh(request, sSphereHandle);

		// request.Materials = mMaterialTables[2];
		// request.Transform = FTransform{{0, 1.f, -2}};

		// mSceneRenderer->SubmitMesh(request);

		FMeshSubmissionRequest request{};
		request.Materials = mMaterialTables[1];
		request.Transform = FTransform{{0, 0, 0}};
		request.Mesh = plane;

		mSceneRenderer->SubmitMesh(request, sPlaneHandle);
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
			mCameras[0].Resize(mViewportSize.x, mViewportSize.y);
			mCameras[1].Resize(mViewportSize.x, mViewportSize.y);
		}

		mSceneRenderer->Begin(&mCameras[0], mCameras[0].GetView());

		FView viewOverride = FView::Create(mCameras[1].GetProjection(), mCameras[1].GetView());
		mSceneRenderer->SetViewOverride(viewOverride);

		DirectionalLight main{};
		main.SetColor(FColor::White).SetIntensity(1.0f).SetDirection({0.f, -1.0f, 0.5f});
		mSceneRenderer->Submit(main);

		PointLight light{};
		light.SetColor(FColor::White).SetIntensity(1.0f).SetRadius(10.f).SetPosition({0, 2, 0});
		renderer.Line.DrawSphere(light.GetRadius(), 20, {}, light.GetColor(), light.GetPosition());
		renderer.Line.DrawGrid({});

		// mSceneRenderer->UpdateTransform(sSphereHandle, sphereTransform);

		// for (uint32_t i = 0; i < transforms.size(); ++i)
		// {
		// 	auto &t = transforms[i];
		// 	auto aabb = mMesh->GetBoundingBox();
		// 	auto &model = t.ToMat4();
		// 	auto pos = t.GetTranslation();
		// 	auto localCenter = aabb.GetCenter();
		// 	auto worldCenter = glm::vec3(model * glm::vec4(localCenter, 1.0f));
		// 	auto scale = glm::vec3(glm::length(glm::vec3(model[0].xyz)), glm::length(glm::vec3(model[1].xyz)), glm::length(glm::vec3(model[2].xyz)));
		// 	float scaledRadius = aabb.GetRadius() * glm::compMax(scale);

		// 	renderer.Line.SetLineWidth(.5f);
		// 	renderer.Line.DrawSphere(scaledRadius, 32, {}, FColor::Purple, {worldCenter});

		// 	renderer.Line.DrawBox(aabb.GetExtent(), {}, FColor::Red, {worldCenter});

		// 	LOG_TRACE("ID {} : Center {} , Radius{} ", i, worldCenter, scaledRadius);
		// }

		// for (uint32_t i = 0; i < 6; ++i)
		// {
		// 	auto pos = mCameras[0].GetView()[3];
		// 	auto &f = mSceneRenderer->GetFrustrum();
		// 	auto plane = f.GetPlanes()[i];
		// 	glm::vec3 n = plane.xyz;
		// 	glm::vec3 p = f.GetPoints()[i];
		// 	renderer.Line.DrawLine(p, p + n * 3.0f, FColor::Yellow);
		// 	LOG_TRACE("Plane {} normal: {}", i, glm::vec3(plane.xyz));
		// }

		// LOG_TRACE("Camera0 - Forward: {} Up: {}", mCameras[0].GetForward(), mCameras[0].GetUp());
		// if (mMesh)
		// {
		// 	auto aabb = mMesh->GetBoundingBox();

		// 	renderer.Line.DrawAABB(aabb, FColor::Orange, sphereTransform);
		// 	renderer.Line.DrawAABB(aabb, FColor::Orange, FTransform{{0, 1.f, -2}});

		// 	renderer.Line.DrawSphere(aabb.GetRadius(), 32, {}, FColor::Purple, sphereTransform);
		// 	renderer.Line.DrawSphere(aabb.GetRadius(), 32, {}, FColor::Purple, FTransform{{0, 1.f, -2}});
		// }

		// if (mPlane)
		// {
		// 	renderer.Line.DrawAABB(mMesh->GetBoundingBox(), FColor::Orange, FTransform{});
		// }

		mSceneRenderer->Submit(light);
		mSceneRenderer->End();
	}

	void SceneLayer::OnGuiRender()
	{
		if (ImGui::Begin("SceneRenderer"))
		{
			auto viewportSize = ImGui::GetContentRegionAvail();
			mViewportSize = {uint32_t(glm::round(viewportSize.x)), uint32_t(glm::round(viewportSize.y))};

			auto cursorPos = ImGui::GetCursorPos();
			auto output = mSceneRenderer->GetOutput();
			if (output)
			{
				auto id = IImGuiTextureProvider::GetID(*output.As<Texture>());
				ImGui::Image(id, viewportSize);
			}

			ImGui::SetCursorPos(cursorPos);
			ImGui::BeginGroup();
			ImGui::Text("Viewport Size: %d x %d", mViewportSize.x, mViewportSize.y);
			ImGui::EndGroup();
		}

		mViewportActive = ImGui::IsWindowHovered() || ImGui::IsWindowFocused();
		Application::Get().BlockImGuiEvents(!mViewportActive);

		ImGui::End();

		if (ImGui::Begin("Actions"))
		{
			if (ImGui::Button("Remove Sphere"))
				mSceneRenderer->UpdateMesh(sSphereHandle, {});

			if (ImGui::Button("Load Mesh"))
			{
				auto info = Platform::OpenFile("Mesh (*.glb;*.gltf)\0*.glb;*.gltf\0");
				if (info)
				{
					FMeshImportOptions import_options{.OverideMaterials = mMaterialTables[0]};

					auto decoded = MeshImporter::Import(info);
					MeshImportResolver resolver(import_options);
					mMesh = resolver.Resolve(decoded);

					mMesh.As<BaseMesh>()->GetMaterialTable() = mMaterialTables[0];
					mSceneRenderer->UpdateMesh(sSphereHandle, mMesh);
				}
			}
		}

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

	void SceneLayer::OnEvent(Event &e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch(this, &SceneLayer::OnKeyEvent);
	}

	bool SceneLayer::OnKeyEvent(KeyEvent &e)
	{
		if (e.Action == EventStatus::PRESS)
		{
			if (e.Key == EKey::Left)
				mCurrentCameraIndex = (mCurrentCameraIndex - 1) % 2;

			if (e.Key == EKey::Right)
				mCurrentCameraIndex = (mCurrentCameraIndex + 1) % 2;

			mCameraController.SetCamera(&mCameras[mCurrentCameraIndex]);
		}

		return false;
	}

} // namespace BHive