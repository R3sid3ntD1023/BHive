#include "SceneLayer.h"

#include "Inspectors/Inspect.h"
#include "core/Application.h"
#include "core/FPSCounter.h"
#include "core/WindowInput.h"
#include "core/layers/ImGuiLayer.h"
#include "core/platform/Platform.h"
#include "core/threading/Threading.h"
#include "gfx/animation/AnimationClip.h"
#include "gfx/factories/MaterialFactory.h"
#include "gfx/factories/MeshFactory.h"
#include "gfx/factories/TextureFactory.h"
#include "gfx/imgui/IImGuiProvider.h"
#include "gfx/material/LambertMaterial.h"
#include "gfx/material/StandardMaterial.h"
#include "gfx/renderers/SceneRenderer.h"
#include "gfx/renderers/postprocess/AcesMaterial.h"
#include "gfx/renderers/postprocess/BloomMaterial.h"
#include "gfx/renderers/postprocess/ColorGradingMaterial.h"
#include "gui/Gui.h"
#include "importers/MeshImportResolver.h"
#include "importers/MeshImporter.h"
#include "importers/TextureImporter.h"

namespace BHive
{
	DirectionalLight main{};
	PointLight light{};
	SpotLight spotLight{};
	std::vector<std::pair<ContextHandle, FTransform>> mTransforms;

	void SceneLayer::OnAttach(Application &app)
	{
		mFont = FontFactory::Create(ENGINE_PATH "/data/fonts/Roboto/Roboto-Regular.ttf", 16.f);

		auto decodeEnvironment = TextureLoader::FromFile(ENGINE_PATH "/data/hdr/kloofendal_43d_clear_puresky_1k.hdr");
		auto decodedSprite = TextureLoader::FromFile("C:/Users/dariu/Documents/BHive/projects/Mario/resources/sprites0.jpg");
		auto decodedMario = TextureLoader::FromFile("C:/Users/dariu/Documents/BHive/projects/Mario/resources/textures/Mario.png");

		auto &window = app.GetWindow();
		auto aspect = window.GetAspectRatio();

		mViewportSize = window.GetSize();

		CameraSensitivity sensitivity{};
		sensitivity.ZoomSpeed = EngineConfig::ZoomSpeed;
		sensitivity.PanSpeed = EngineConfig::PanSpeed;
		sensitivity.MoveSpeed = EngineConfig::MoveSpeed;
		sensitivity.OrbitSpeed = EngineConfig::OrbitSpeed;

		mCameras[0] = EditorCamera(75.f, aspect, 0.1f, 1000.f);
		mCameras[0].SetStartState({0.f, 10.f, 10.f}, -90.0f, -35.0f);
		mCameras[0].SetSensitivity(sensitivity);

		mCameras[1] = EditorCamera(75.f, aspect, 0.1f, 1000.f);
		mCameras[1].SetStartState({0.f, 10.f, 10.f}, -90.0f, -35.0f);
		mCameras[1].SetSensitivity(sensitivity);

		mSceneRenderer = CreateRef<SceneRenderer>();
		mSceneRenderer->Init(mViewportSize);
		mSceneRenderer->SetEnvironmentTexture(TextureFactory::Create2D(decodeEnvironment));

		mSceneRenderer->AddPostProcessMaterial<BloomMaterial>();
		mSceneRenderer->AddPostProcessMaterial<AcesMaterial>();
		mSceneRenderer->AddPostProcessMaterial<ColorGradingMaterial>();

		auto mesh = MeshFactory::CreateSphere(1.0f, 32u, 32u);
		auto plane = MeshFactory::CreatePlane(1.f, 1.f);
		mTexture = TextureFactory::Create2D(decodedSprite);

		{

			auto sphereMat = MaterialFactory::CreateStandard();
			auto planeMat = MaterialFactory::CreateStandard();
			auto transparentMat = MaterialFactory::CreateStandard();

			planeMat.As<StandardMaterial>()->SetFlags(StandardMaterial::EFlags::ReceiveShadows);

			auto texture = TextureFactory::Create2D(decodedMario);
			transparentMat.As<StandardMaterial>()->SetSurfaceType(Material::ESurfaceType::Transparent);
			transparentMat.As<StandardMaterial>()->SetTexture("DiffuseMap", {texture});

			auto mat = sphereMat.As<StandardMaterial>();
			mat->SetAlbedo({0.5f, 0.5f, 0.5f, 1.0f});
			mat->SetEmission(FColor::Black);
			mat->SetMetalness(0.0f);
			mat->SetRoughness(0.5f);

			mMaterialTables.emplace_back().Add(planeMat);
			mMaterialTables.emplace_back().Add(sphereMat);
			mMaterialTables.emplace_back().Add(transparentMat);
		}

#if 0
	#define TEST_MESH_NAME "C://Users//dariu//Documents//MultiSubMesh.glb"
	#define SCALE 1.0f
#else
	#define TEST_MESH_NAME "C://Users//dariu//Documents//BHive//projects//Shadows//resources//Kachujin//Kachujin.gltf"
	#define TEST_ANIMATION "C://Users//dariu//Documents//BHive//projects//Shadows//resources//Kachujin//animations//Unarmed Idle 02.glb"
	#define SCALE .05f
#endif

		mCameraController.SetCamera(&mCameras[0]);

		// lights
		main.SetColor(FColor::White).SetIntensity(1.0f).SetDirection({-1.0f, -1.0f, 0.0f});
		light.SetColor(FColor::Orange).SetIntensity(1.0f).SetRadius(10.f).SetPosition({0, 1, 0});
		spotLight.SetColor(FColor::Brown).SetIntensity(10.0f).SetRadius(10.0f).SetDirection({0.f, -1.f, 0.f}).SetPosition({1, 5, 0});

		uint32_t count = 0;
		for (int32_t i = -1; i <= 1; i++)
		{
			for (int32_t j = -1; j <= 1; j++, count++)
			{
				mTransforms.emplace_back(std::pair{ContextHandle{}, FTransform{{i * 3.0f, 1.5f, j * 3.0f}}});

				FMeshSubmissionRequest request{};
				request.Mesh = mesh;
				request.Materials = mMaterialTables[1];
				request.Transform = mTransforms[count].second;
				mSceneRenderer->SubmitMesh(request, mTransforms.at(count).first);
			}
		}

		mTransforms.emplace_back(std::pair{ContextHandle{}, FTransform{{0, -.5, 0}, {}, {50.f, 1.f, 50.f}}});
		mTransforms.emplace_back(std::pair{ContextHandle{}, FTransform{{2, 1.5, 0}}});
		mTransforms.emplace_back(std::pair{ContextHandle{}, FTransform{{0, 0, 0}}});

		{
			FMeshSubmissionRequest request{};
			request.Materials = mMaterialTables[0];
			request.Transform = mTransforms[9].second;
			request.Mesh = plane;
			mSceneRenderer->SubmitMesh(request, mTransforms.at(9).first);
		}

		{
			FMeshSubmissionRequest request{};
			request.Materials = mMaterialTables[2];
			request.Transform = mTransforms[10].second;
			request.Mesh = plane;
			mSceneRenderer->SubmitMesh(request, mTransforms.at(10).first);
		}

		{
			FMeshImportOptions import_options{};
			import_options.MeshType = EMeshType::SkeletalMesh;
			auto decodedMesh = MeshImporter::Import(TEST_MESH_NAME, SCALE);
			MeshImportResolver resolver(import_options);
			auto result = resolver.Resolve(decodedMesh);
			mCharacter = result.Mesh;
			mMaterialTables.emplace_back(result.Materials);

#ifdef TEST_ANIMATION
			mCharacterSkeleton = result.Skeleton;
			import_options.MeshType = EMeshType::SkeletalAnimation;
			import_options.ImportMaterials = false;
			import_options.Skeleton = mCharacterSkeleton;
			auto decodedAnimation = MeshImporter::Import(TEST_ANIMATION, SCALE);
			resolver.SetOptions(import_options);
			result = resolver.Resolve(decodedAnimation);
			mCharacterAnimaton = result.Animations[0];
			mCharacterPose = mCharacter.As<SkeletalMesh>()->GetDefaultPose();
			mAnimationClip = CreateRef<AnimationClip>(mCharacterAnimaton, mCharacterSkeleton);
#endif

			if (mCharacter)
			{

				FMeshSubmissionRequest request{};
				request.Mesh = mCharacter;
				request.Materials = mMaterialTables.back();
				request.Transform = mTransforms[11].second;
				request.BoneTransforms = mCharacter.As<SkeletalMesh>()->GetSkeleton()->GetRestPoseTransforms();
				mSceneRenderer->SubmitMesh(request, mTransforms.at(11).first);
			}
		}
	}

	void SceneLayer::OnDetach()
	{
	}

	void SceneLayer::OnUpdate(float time)
	{
		if (mAnimationClip && mCharacter)
		{
			auto &pose = *mCharacterPose;
			mAnimationClip->Play(time, pose);
			mSceneRenderer->UpdateTransform(mTransforms[11].first, mTransforms[11].second);
			mSceneRenderer->UpdateBones(mTransforms[11].first, pose.GetTransformsJointSpace());
		}

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
		mSceneRenderer->Submit(main);
		mSceneRenderer->Submit(light);
		mSceneRenderer->Submit(spotLight);

		auto view = mSceneRenderer->GetSceneView().View;

		// FView viewOverride = FView::Create(mCameras[1].GetProjection(), mCameras[1].GetView());
		// mSceneRenderer->SetViewOverride(viewOverride);

		renderer.Line.DrawSphere(light.GetRadius(), 20, {}, light.GetColor(), {light.GetPosition()});
		renderer.Line.DrawGrid({});
		renderer.Line.DrawLine({0, 0, 0}, {10, 0, 0}, FColor::Red);
		renderer.Line.DrawLine({0, 0, 0}, {0, 10, 0}, FColor::Green);
		renderer.Line.DrawLine({0, 0, 0}, {0, 0, 10}, FColor::Blue);
		renderer.Line.DrawLine({0, 0, 0}, main.GetDirection() * 5.0f, main.GetColor(), FTransform{{-5, 5, 0}});
		renderer.Line.DrawCircle(2.0f, 32, {}, FColor::Red, FTransform{{0, 3, 0}});
		renderer.Line.DrawCircle(4.0f, 32, {}, FColor::Blue, FTransform{{0, 3, 0}});

		renderer.Quad.DrawQuad(FQuadParams{}, mTexture, FTransform{{-2, 4, 0}});

		renderer.Quad.DrawBillboard(view, FQuadParams{}, mTexture, FTransform{{2, 4, 0}});

		renderer.Quad.DrawText(mFont, 2.f, "Test Text", FTextParams{}, FTransform{{2, 2, 0}});

		renderer.Quad.DrawCircle(FCircleParams{}, FTransform{{-2, 2, 0}});

		renderer.Line.DrawSpotlightCone(spotLight.GetPosition(), spotLight.GetDirection(), spotLight.GetRadius(), spotLight.GetOuterAngleDegrees(), 32, spotLight.GetColor());

		mSceneRenderer->End();
	}

	void SceneLayer::OnGuiRender()
	{
		const float fps = FPSCounter::Get();

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

		if (ImGui::Begin("Transforms"))
		{
			for (uint32_t i = 0; i < mTransforms.size(); ++i)
			{
				auto name = std::format("Transform_{}", i);
				auto &[c, t] = mTransforms[i];
				if (Inspect::get().inspect(name, t))
					mSceneRenderer->UpdateTransform(c, t);
			}
		}

		ImGui::End();

		if (ImGui::Begin("Materials"))
		{
			for (uint32_t t = 0; t < mMaterialTables.size(); t++)
			{
				auto &table = mMaterialTables[t];
				auto label = std::format("MaterialTable_{}", t);
				bool opened = ImGui::TreeNodeEx(label.c_str(), ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_LabelSpanAllColumns);

				if (opened)
				{
					for (uint32_t m = 0; m < table.Count(); m++)
					{
						auto name = std::format("Material_{}", m);
						auto mat = table[m].As<StandardMaterial>();
						Inspect::get().inspect(name, *mat);
					}
					ImGui::TreePop();
				}
			}
		}

		ImGui::End();

		if (ImGui::Begin("FPS"))
		{
			ImGui::Text("CPU FPS: %.2f", fps);
		}

		ImGui::End();

		if (ImGui::Begin("Lights"))
		{
			Inspect::get().inspect("MainLight", main);
			Inspect::get().inspect("PointLight", light);
			Inspect::get().inspect("SpotLight", spotLight);
		}

		ImGui::End();

		if (ImGui::Begin("Actions"))
		{

			if (ImGui::Button("Load HDR Environment"))
			{
				auto info = Platform::OpenFile("HDR Environment (*.hdr)//0*.hdr//0");
				if (info)
				{
					auto decoded = TextureLoader::FromFile(info);
					mSceneRenderer->SetEnvironmentTexture(TextureFactory::Create2D(decoded));
				}
			}

			if (ImGui::Button("Load Mesh"))
			{
				auto fileInfo = Platform::OpenFile("(obj)\0*.obj\0(glb)\0*.glb\0(fbx)\0*.fbx\0");
				if (fileInfo)
				{
					FMeshImportOptions options{.ImportMaterials = true};
					auto decodedMesh = MeshImporter::Import(fileInfo, 0.001f);
					MeshImportResolver resolver(options);
					auto result = resolver.Resolve(decodedMesh);

					auto &newT = mTransforms.emplace_back();
					FMeshSubmissionRequest request;
					request.Mesh = result.Mesh;
					request.Materials = result.Materials;
					request.Transform = newT.second;
					mSceneRenderer->SubmitMesh(request, newT.first);
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