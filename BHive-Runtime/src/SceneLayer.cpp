#include "SceneLayer.h"

#include "Inspectors/Inspect.h"
#include "core/Application.h"
#include "core/WindowInput.h"
#include "core/layers/ImGuiLayer.h"
#include "core/platform/Platform.h"
#include "gfx/Framebuffer.h"
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
	FTransform sphereTransform{{0, 0, 0}};
	std::vector<FTransform> transforms;
	std::array<ContextHandle, 9> sSphereHandle;
	ContextHandle sPlaneHandle;
	ContextHandle sCharacterHandle;

	void SceneLayer::OnAttach(Application &app)
	{
		auto decodeEnvironment = TextureLoader::FromFile(ENGINE_PATH "/data/hdr/kloofendal_43d_clear_puresky_1k.hdr");
		auto decodedSprite = TextureLoader::FromFile("C:/Users/dariu/Documents/BHive/projects/Mario/resources/sprites0.jpg");
		auto decodedMario = TextureLoader::FromFile("C:/Users/dariu/Documents/BHive/projects/Mario/resources/textures/Mario.png");

		auto &window = app.GetWindow();
		auto aspect = window.GetAspectRatio();

		mViewportSize = window.GetSize();

		mCameras[0] = EditorCamera(75.f, aspect, 0.1f, 1000.f);
		mCameras[0].SetStartState({0.f, 0.f, 0.f}, -90.0f, 0.0f);

		mCameras[1] = EditorCamera(75.f, aspect, 0.1f, 1000.f);
		mCameras[1].SetStartState({0.f, 10.f, 10.f}, -90.0f, -45.0f);

		mSceneRenderer = CreateRef<SceneRenderer>();
		mSceneRenderer->Init(mViewportSize);
		mSceneRenderer->SetEnvironmentTexture(TextureFactory::Create2D(decodeEnvironment));

		// mSceneRenderer->AddPostProcessMaterial<BloomMaterial>();
		mSceneRenderer->AddPostProcessMaterial<AcesMaterial>();
		// mSceneRenderer->AddPostProcessMaterial<ColorGradingMaterial>();

		auto mesh = MeshFactory::CreateSphere(1.0f, 32u, 32u);
		auto plane = MeshFactory::CreatePlane(10.f, 10.f);

		{

			mStandardMaterial = MaterialFactory::CreateStandard();

			mMaterialTables[0].Add(MaterialFactory::CreateLambert());
			mMaterialTables[1].Add(MaterialFactory::CreateLambert());
			mMaterialTables[2].Add(mStandardMaterial);

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
				material->SetAlbedo({0.5f, 0.5f, 0.5f, 1.0f});
				material->SetEmission(FColor::Black);
				material->SetMetalness(0.0f);
				material->SetRoughness(0.5f);
			}

#if 0
	#define TEST_MESH_NAME "C://Users//dariu//Documents//MultiSubMesh.glb"
	#define SCALE 1.0f
#else
	#define TEST_MESH_NAME "C://Users//dariu//Documents//BHive//projects//Shadows//resources//Kachujin//Kachujin.gltf"
	#define TEST_ANIMATION "C://Users//dariu//Documents//BHive//projects//Shadows//resources//Kachujin//animations//Unarmed Idle 02.glb"
	#define SCALE .05f
#endif
			{
				FMeshImportOptions import_options{};
				import_options.MeshType = EMeshType::SkeletalMesh;
				import_options.OverrideMaterials = mCharacterMaterials;
				auto decodedMesh = MeshImporter::Import(TEST_MESH_NAME, SCALE);
				MeshImportResolver resolver(import_options);
				auto result = resolver.Resolve(decodedMesh);
				mCharacter = result.Mesh;
				mCharacterMaterials = result.Materials;
				mCharacterMaterials.SetAll(mStandardMaterial);

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
				LOG_INFO("Animation {} Duration {}, Length {}", mCharacterAnimaton.Index, mAnimationClip->GetDuration(), mAnimationClip->GetLengthInSeconds());
#endif
			}
		}

		mCameraController.SetCamera(&mCameras[0]);

		uint32_t count = 0;
		for (int32_t i = -1; i <= 1; i++)
		{
			for (int32_t j = -1; j <= 1; j++, count++)
			{
				FMeshSubmissionRequest request{};
				request.Mesh = mesh;
				request.Materials = mMaterialTables[2];
				request.Transform = transforms.emplace_back(FTransform{{i * 3.0f, 0.0f, j * 3.0f}});
				mSceneRenderer->SubmitMesh(request, sSphereHandle[count]);
			}
		}

		FMeshSubmissionRequest request{};
		request.Materials = mMaterialTables[2];
		request.Transform = FTransform{{0, 0, 0}};
		request.Mesh = plane;
		mSceneRenderer->SubmitMesh(request, sPlaneHandle);

		if (mCharacter)
		{

			FMeshSubmissionRequest request{};
			request.Mesh = mCharacter;
			request.Materials = mCharacterMaterials;
			request.Transform = sphereTransform;
			request.BoneTransforms = mCharacter.As<SkeletalMesh>()->GetSkeleton()->GetRestPoseTransforms();
			mSceneRenderer->SubmitMesh(request, sCharacterHandle);
		}
	}

	void SceneLayer::OnDetach()
	{
	}

	void SceneLayer::OnUpdate(float time)
	{
		sphereTransform.AddRotation({0, 10.f * time, 0});

		if (mAnimationClip)
		{
			auto &pose = *mCharacterPose;
			mAnimationClip->Play(time, pose);
			mSceneRenderer->UpdateBones(sCharacterHandle, pose.GetTransformsJointSpace());
			mSceneRenderer->UpdateTransform(sCharacterHandle, sphereTransform);

			// LOG_INFO("{}", glm::vec3(pose.GetTransformsJointSpace()[0][3]));
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

		FView viewOverride = FView::Create(mCameras[1].GetProjection(), mCameras[1].GetView());
		mSceneRenderer->SetViewOverride(viewOverride);

		DirectionalLight main{};
		main.SetColor(FColor::White).SetIntensity(1.0f).SetDirection({0.f, -1.0f, 0.5f});
		mSceneRenderer->Submit(main);

		PointLight light{};
		light.SetColor(FColor::Orange).SetIntensity(1.0f).SetRadius(10.f).SetPosition({0, 1, 0});
		mSceneRenderer->Submit(light);
		renderer.Line.DrawSphere(light.GetRadius(), 20, {}, light.GetColor(), light.GetPosition());
		renderer.Line.DrawGrid({});
		renderer.Line.DrawLine({0, 0, 0}, {10, 0, 0}, FColor::Red);
		renderer.Line.DrawLine({0, 0, 0}, {0, 10, 0}, FColor::Green);
		renderer.Line.DrawLine({0, 0, 0}, {0, 0, 10}, FColor::Blue);

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
				mSceneRenderer->UpdateMesh(sSphereHandle[0], {});

			if (ImGui::Button("Load Mesh"))
			{
				auto info = Platform::OpenFile("Mesh (*.glb;*.gltf)//0*.glb;*.gltf//0");
				if (info)
				{
					FMeshImportOptions import_options{};
					import_options.OverrideMaterials = mCharacterMaterials;
					import_options.MeshType = EMeshType::StaticMesh;

					auto decoded = MeshImporter::Import(info);
					MeshImportResolver resolver(import_options);
					auto result = resolver.Resolve(decoded);
					mMesh = result.Mesh;
					mSceneRenderer->UpdateMesh(sSphereHandle[0], mMesh);
				}
			}

			auto loadTexture = [&](const std::string &name)
			{
				if (ImGui::Button(name.c_str()))
				{
					auto info = Platform::OpenFile("Texture (*.png;*.jpeg)//0*.png;*.jpeg//0");
					if (info)
					{
						auto decoded = TextureLoader::FromFile(info);
						auto texture = TextureFactory::Create2D(decoded);
						mStandardMaterial.As<Material>()->SetTexture(std::format("{}Map", name), {texture});
					}
				}
			};

			auto inspect = [&](const std::string label)
			{
				auto mat = mStandardMaterial.As<StandardMaterial>();
				Inspect::get().inspect(label, *mat);
			};

			loadTexture("Diffuse");
			loadTexture("Metalness");
			loadTexture("Roughness");
			loadTexture("Normal");
			loadTexture("Emission");
			loadTexture("Opacity");
			inspect("Material");

			if (ImGui::Button("Load HDR Environment"))
			{
				auto info = Platform::OpenFile("HDR Environment (*.hdr)//0*.hdr//0");
				if (info)
				{
					auto decoded = TextureLoader::FromFile(info);
					mSceneRenderer->SetEnvironmentTexture(TextureFactory::Create2D(decoded));
				}
			}

			if (Inspect::get().inspect("Transform", sphereTransform))
				mSceneRenderer->UpdateTransform(sCharacterHandle, sphereTransform);
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