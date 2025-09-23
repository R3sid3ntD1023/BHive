#include "SandboxLayer.h"
#include "core/Application.h"
#include "mesh/primitives/Cube.h"
#include "mesh/MeshImporter.h"
#include "mesh/Skeleton.h"
#include "mesh/SkeletalMesh.h"
#include "mesh/SkeletalAnimation.h"
#include "mesh/AnimationClip.h"
#include "mesh/SkeletalPose.h"
#include "gfx/ShaderManager.h"
#include "gui/ImGuiExtended.h"
#include "renderers/SceneRenderer.h"
#include "material/StandardMaterial.h"
#include "material/LambertMaterial.h"
#include "mesh/MeshImportResolver.h"
#include "core/FPSCounter.h"

namespace BHive
{
	glm::vec3 lightpos = {-5, 10, 0};
	float lightRadius = 50.f;

	FTransform sSpotTransform{};

	FTransform sDirectionalLightTransform{};

	float GetRandomDisplacement(float offset)
	{
		return (rand() % (int)(2 * offset * 100)) / 100.f - offset;
	}

	void SandboxLayer::OnAttach()
	{
		mInstances.resize(1000);
		for (uint32_t i = 0; i < 1000; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			float angle = (float)i / 1000.0f * 360.f;
			float displacement = GetRandomDisplacement(offset);
			float x = sin(angle) * radius + displacement;
			displacement = GetRandomDisplacement(offset);
			float y = displacement * 0.4f;
			displacement = GetRandomDisplacement(offset);
			float z = cos(angle) * radius + displacement;
			model = glm::translate(model, glm::vec3(x, y, z));

			mInstances[i] = model;
		}

		auto &window = Application::Get().GetWindow();

		mCamera = EditorCamera(45.0f, window.GetAspectRatio(), .01f, 1000.f);

		auto material = CreateRef<LambertMaterial>();
		mPlane = CreateRef<PCube>(1.f);
		mPlane->GetMaterialTable().add_material(material);

		{
			FMeshImportData data;
			if (MeshImporter::Import(RESOURCE_PATH "industrial_standing_light/scene.gltf", data))
			{

				mLightPost = CreateRef<StaticMesh>(data.mMeshData);

				for (size_t i = 0; i < data.mMaterialData.size(); i++)
				{
					mLightPost->GetMaterialTable().add_material(material);
				}
			}
		}

		{
			FMeshImportData data;
			if (MeshImporter::Import(RESOURCE_PATH "Kachujin/Kachujin.gltf", data))
			{
				mSkeleton = CreateRef<Skeleton>(data.mBoneData, data.mSkeletonHeirarchyData);
				mCharacter = CreateRef<SkeletalMesh>(data.mMeshData, mSkeleton);
				for (size_t i = 0; i < data.mMaterialData.size(); i++)
				{
					mCharacter->GetMaterialTable().add_material(material);
				}
				mPose = mCharacter->GetDefaultPose();
			}
		}

		{
			FMeshImportData data;
			if (MeshImporter::Import(RESOURCE_PATH "Kachujin/animations/Unarmed Idle 01.glb", data))
			{
				auto &anim = data.mAnimationData[0];
				mAnimation = CreateRef<SkeletalAnimation>(anim.mDuration, anim.TicksPerSecond, anim.mFrames, mSkeleton, anim.mGlobalInverseMatrix);

				mAnimationClip = CreateRef<AnimationClip>(mAnimation);
			}
		}

		mRenderer = CreateRef<SceneRenderer>();
		mRenderer->Init(window.GetSize());
	}

	void SandboxLayer::OnUpdate(float dt)
	{
		FPSCounter::Get().Frame();

		if (mAnimationClip)
		{
			mAnimationClip->Play(dt, *mPose);
		}

		mCamera.ProcessInput();

		FPointLightCreateInfo light{};
		light.Position = {0, 1, 2};
		light.Color = {1, 1, 1};
		light.Radius = lightRadius;

		FSpotLightCreateInfo spotLight{};
		spotLight.Position = sSpotTransform.GetTranslation();
		spotLight.Direction = sSpotTransform.GetForward();
		spotLight.Radius = 30.0f;

		FDirectionalLightCreateInfo directionalLight{};
		directionalLight.Direction = sDirectionalLightTransform.GetForward();
		directionalLight.Color = {1, 1, 1};

		mRenderer->Begin(&mCamera, mCamera.GetView());

		mRenderer->SubmitLight(directionalLight);
		mRenderer->SubmitLight(spotLight);
		mRenderer->SubmitLight(light);

		std::vector<glm::mat4> matrices = {FTransform({-4, .5, 0}), FTransform({5, -.05, 0})};
		FMeshInfo mesh_info_post{};
		mesh_info_post.Mesh = mLightPost;
		mesh_info_post.Materials = mLightPost->GetMaterialTable();
		mesh_info_post.Instances = {matrices};

		std::vector<glm::mat4> matrices2 = {FTransform({-10, 0, 0}), FTransform({10, 0, 0})};
		FMeshInfo plane_info{};
		plane_info.Mesh = mPlane;
		plane_info.Materials = mPlane->GetMaterialTable();
		plane_info.Instances = {mInstances};

		mRenderer->SubmitMesh(mesh_info_post);
		mRenderer->SubmitMesh(plane_info);

		if (mCharacter)
		{
			auto character_transform = FTransform({0, 0, 0}, {-90, 0, 0}, {.01, .01, .01});

			FMeshInfo character_info{};
			character_info.Mesh = mCharacter;
			character_info.Materials = mCharacter->GetMaterialTable();
			character_info.Transform = character_transform;
			character_info.Bones.Bones = mPose->GetTransformsJointSpace();

			mRenderer->SubmitMesh(character_info);
		}

		mRenderer->SubmitCommand([]() { LineRenderer::DrawGrid({.color = {1, .5f, 0, 1}, .stepcolor = {1, .5f, .3f, 1}}); });
		mRenderer->End();

		mRenderer->RenderToScreen();
	}

	void SandboxLayer::OnDetach()
	{
	}

	void SandboxLayer::OnEvent(Event &e)
	{
		mCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch(this, &SandboxLayer::OnWindowResize);
	}

	void SandboxLayer::OnGuiRender()
	{
		float fps = FPSCounter::Get();
		ImGui::Begin("FPS");
		ImGui::TextColored({1, .5, 0, 1}, "%.2f", fps);
		ImGui::End();
	}

	bool SandboxLayer::OnWindowResize(WindowResizeEvent &e)
	{
		mCamera.Resize(e.x, e.y);
		mRenderer->Resize({e.x, e.y});

		return false;
	}

} // namespace BHive