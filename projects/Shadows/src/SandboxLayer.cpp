#include "SandboxLayer.h"
#include "renderers/Renderer.h"
#include "core/Application.h"
#include "gfx/RenderCommand.h"
#include "mesh/primitives/Cube.h"
#include "gfx/Shader.h"
#include "mesh/indirect_mesh/IndirectMesh.h"
#include "mesh/MeshImporter.h"
#include "mesh/Skeleton.h"
#include "mesh/SkeletalMesh.h"
#include "mesh/SkeletalAnimation.h"
#include "mesh/AnimationClip.h"
#include "mesh/SkeletalPose.h"

namespace BHive
{
	void SandboxLayer::OnAttach()
	{
		auto &window = Application::Get().GetWindow();
		auto windowSize = window.GetSize();
		auto aspect = windowSize.x / (float)windowSize.y;

		mCamera = EditorCamera(45.0f, aspect, .01f, 1000.f);

		RenderCommand::ClearColor(.1f, .1f, .1f, 1.0f);

		mPlane = CreateRef<PCube>(1.f);
		mIndirectPlane = CreateRef<IndirectRenderable>();
		mIndirectPlane->Init(mPlane, 2);

		{
			FMeshImportData data;
			if (MeshImporter::Import(RESOURCE_PATH "industrial_standing_light/scene.gltf", data))
			{
				mLightPost = CreateRef<StaticMesh>(data.mMeshData);
				mIndirectLightPost = CreateRef<IndirectRenderable>();
				mIndirectLightPost->Init(mLightPost, 2);
			}
		}

		{
			FMeshImportData data;
			if (MeshImporter::Import(RESOURCE_PATH "Kachujin/Kachujin.gltf", data))
			{
				mSkeleton = CreateRef<Skeleton>(data.mBoneData, data.mSkeletonHeirarchyData);
				mCharacter = CreateRef<SkeletalMesh>(data.mMeshData, mSkeleton);
				mIndirectCharacter = CreateRef<IndirectRenderable>();
				mIndirectCharacter->Init(mCharacter, 1, true);
			}
		}

		{
			FMeshImportData data;
			if (MeshImporter::Import(RESOURCE_PATH "Kachujin/animations/Unarmed Idle 01.glb", data))
			{
				auto &anim = data.mAnimationData[0];
				mAnimation = CreateRef<SkeletalAnimation>(
					anim.mDuration, anim.TicksPerSecond, anim.mFrames, mSkeleton, anim.mGlobalInverseMatrix);

				mAnimationClip = CreateRef<AnimationClip>(mAnimation);
				mPose = CreateRef<SkeletalPose>(mSkeleton.get());
			}
		}

		mShader = ShaderLibrary::Load(RESOURCE_PATH "ShadowShader.glsl");
	}

	void SandboxLayer::OnUpdate(float dt)
	{
		mCamera.ProcessInput();

		RenderCommand::Clear();

		Renderer::Begin(mCamera.GetProjection(), mCamera.GetView().inverse());

		LineRenderer::DrawGrid({});

		mShader->Bind();
		// mShader->SetUniform("uColor", glm::vec3(.5, .5, .5));

		glm::mat4 matrices[] = {FTransform({-10, 0, 0}), FTransform({10, 0, 0})};
		mIndirectPlane->Draw(FTransform({0, -.05, 0}, {}, {10, .1, 10}), matrices);

		// mShader->SetUniform("uColor", glm::vec3(1, .5f, 0));
		mIndirectLightPost->Draw(FTransform({}, {0, 0, 0}), matrices);

		if (mAnimationClip && mIndirectCharacter)
		{
			mAnimationClip->Play(dt, *mPose);

			auto character_transform = FTransform({5, 0, 0}, {-90, 0, 0}, {.01, .01, .01});
			auto &bones = mPose->GetTransformsJointSpace();
			mIndirectCharacter->Draw(character_transform, nullptr, bones.data());
		}

		mShader->UnBind();

		Renderer::End();
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

	bool SandboxLayer::OnWindowResize(WindowResizeEvent &e)
	{
		mCamera.Resize(e.x, e.y);

		RenderCommand::SetViewport(0, 0, e.x, e.y);

		return false;
	}

} // namespace BHive