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
#include "gfx/ShaderManager.h"
#include "renderers/ShadowRenderer.h"
#include "gfx/Framebuffer.h"
#include "mesh/primitives/Quad.h"
#include "gui/ImGuiExtended.h"

namespace BHive
{
	glm::vec3 lightpos = {-5, 10, 0};
	float lightRadius = 50.f;

	SpotLight sSpotLight{};
	FTransform sSpotTransform{};

	DirectionalLight sDirectionalLight{};
	FTransform sDirectionalLightTransform{};

	void SandboxLayer::OnAttach()
	{
		ShaderManager::Get().LoadFiles(RESOURCE_PATH);
		mShader = ShaderManager::Get().Get("ShadowShader.glsl");
		mScreenQuadShader = ShaderManager::Get().Get("ScreenQuad2.glsl");

		auto &window = Application::Get().GetWindow();
		auto windowSize = window.GetSize();
		auto aspect = windowSize.x / (float)windowSize.y;

		mCamera = EditorCamera(45.0f, aspect, .01f, 1000.f);

		RenderCommand::ClearColor(.1f, .1f, .1f, 1.0f);

		auto material = CreateRef<Material>(mShader);
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
				mAnimation = CreateRef<SkeletalAnimation>(
					anim.mDuration, anim.TicksPerSecond, anim.mFrames, mSkeleton, anim.mGlobalInverseMatrix);

				mAnimationClip = CreateRef<AnimationClip>(mAnimation);
			}
		}

		FramebufferSpecification spec{};
		spec.Width = windowSize.x;
		spec.Height = windowSize.y;
		spec.Attachments.attach(FTextureSpecification{.InternalFormat = EFormat::RGB8, .WrapMode = EWrapMode::CLAMP_TO_EDGE})
			.attach(
				FTextureSpecification{.InternalFormat = EFormat::DEPTH24_STENCIL8, .WrapMode = EWrapMode::CLAMP_TO_EDGE});

		mFramebuffer = CreateRef<Framebuffer>(spec);

		mScreenQuad = CreateRef<PQuad>();
	}

	void SandboxLayer::OnUpdate(float dt)
	{
		if (mAnimationClip)
		{
			mAnimationClip->Play(dt, *mPose);
		}

		mCamera.ProcessInput();

		PointLight light{};
		light.mBrightness = 1.f;
		light.mColor = 0xFFFFFFFF;
		light.mRadius = lightRadius;

		auto &proj = mCamera.GetProjection();
		auto view = mCamera.GetView().inverse();

		Renderer::Begin();
		Renderer::SubmitCamera(proj, view);

		ShadowRenderer::Begin();
		ShadowRenderer::SubmitPointLight(lightpos, light.mRadius);
		ShadowRenderer::SubmitDirectionalLight(sDirectionalLightTransform.get_forward(), proj, view);
		ShadowRenderer::SubmitSpotLight(sSpotTransform.get_forward(), sSpotTransform.get_translation(), sSpotLight.mRadius);

		ShadowRenderer::BeginShadowPass();
		DrawScene();
		ShadowRenderer::EndShadowPass();

		ShadowRenderer::BeginPointShadowPass();
		DrawScene();
		ShadowRenderer::EndPointShadowPass();

		ShadowRenderer::BeginSpotShadowPass();
		DrawScene();
		ShadowRenderer::EndSpotShadowPass();

		ShadowRenderer::End();

		mFramebuffer->Bind();

		RenderCommand::Clear();

		Renderer::SubmitDirectionalLight(sDirectionalLightTransform.get_forward(), sDirectionalLight);
		Renderer::SubmitPointLight(lightpos, light);
		Renderer::SubmitSpotLight(sSpotTransform.get_forward(), sSpotTransform.get_translation(), sSpotLight);

		LineRenderer::DrawGrid({.color = {1, .5f, 0, 1}, .stepcolor = {1, .5f, .3f, 1}});

		mShader->Bind();

		ShadowRenderer::GetShadowFBO()->GetDepthAttachment()->Bind();
		ShadowRenderer::GetPointShadowFBO()->GetColorAttachment()->Bind(1);
		ShadowRenderer::GetSpotShadowFBO()->GetColorAttachment()->Bind(2);

		DrawScene();

		mShader->UnBind();

		Renderer::End();

		mFramebuffer->UnBind();

		RenderCommand::Clear();

		mScreenQuadShader->Bind();
		mFramebuffer->GetColorAttachment()->Bind();

		RenderCommand::DrawElements(EDrawMode::Triangles, *mScreenQuad->GetVertexArray());

		mScreenQuadShader->UnBind();
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
		ImGui::Begin("Test");

		ImGui::DragFloat3("LightPos", &lightpos.x, 0.1f);
		ImGui::DragFloat("LightRadius", &lightRadius, 0.1f);

		ImGui::PushID("Spot");
		ImGui::DragTransform("SpotLight", sSpotTransform);

		ImGui::DragFloat("Brightness", &sSpotLight.mBrightness, 0.1f);
		ImGui::DragFloat("Radius", &sSpotLight.mRadius, 0.1f);
		ImGui::DragFloat("InnerCutoff", &sSpotLight.mInnerCutOff, 0.1f);
		ImGui::DragFloat("OuterCutoff", &sSpotLight.mOuterCutOff, 0.1f);

		ImGui::PopID();

		ImGui::PushID("DirectionalLight");
		ImGui::DragTransform("DirectionalLight", sDirectionalLightTransform);
		ImGui::ColorEdit("Color", sDirectionalLight.mColor);
		ImGui::DragFloat("Brightness", &sDirectionalLight.mBrightness, 0.1f);
		ImGui::PopID();

		ImGui::End();
	}

	bool SandboxLayer::OnWindowResize(WindowResizeEvent &e)
	{
		mCamera.Resize(e.x, e.y);
		mFramebuffer->Resize(e.x, e.y);

		return false;
	}

	void SandboxLayer::DrawScene()
	{
		glm::mat4 matrices[] = {FTransform({-4, .5, 0}, {}, {1, 1, 1}), FTransform({5, -.05, 0}, {}, {40, .1, 20})};
		MeshRenderer::DrawMesh(mLightPost);

		glm::mat4 matrices2[] = {FTransform({-10, 0, 0}), FTransform({10, 0, 0})};
		MeshRenderer::DrawMesh(mPlane, {1.0f}, matrices2, 2);

		if (mCharacter)
		{
			auto character_transform = FTransform({0, 0, 0}, {-90, 0, 0}, {.01, .01, .01});
			MeshRenderer::DrawMesh(mCharacter, *mPose, character_transform);
		}
	}

} // namespace BHive