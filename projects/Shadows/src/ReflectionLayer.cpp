#include "ReflectionLayer.h"
#include "gfx/ShaderManager.h"
#include "renderers/Renderer.h"
#include "core/Application.h"
#include "gfx/RenderCommand.h"
#include "gfx/textures/RenderTargetCube.h"
#include "core/Time.h"
#include "gfx/UniformBuffer.h"

namespace BHive
{
	struct alignas(16) ReflectionMaterial
	{
		glm::vec4 Color;
		float IOR;
		float Reflective;
	};

	void ReflectionLayer::OnAttach()
	{
		mReflectionShader = ShaderManager::Get().Load(RESOURCE_PATH "shaders/reflection.glsl");
		auto &ubo = mReflectionShader->GetRelectionData().UniformBuffers.at("Material");

		mReflectUBO = CreateRef<UniformBuffer>(ubo.Binding, ubo.Size);

		mPlane = CreateRef<PPlane>(100.f, 100.f);
		mSphere = CreateRef<PSphere>(2.f);

		mRelfectionTarget = CreateRef<RenderTargetCube>(256, 100.f);

		mPlaneIndirect = CreateRef<IndirectRenderable>();
		mPlaneIndirect->Init(mPlane);

		mSphereIndirect = CreateRef<IndirectRenderable>();
		mSphereIndirect->Init(mSphere);

		auto size = Application::Get().GetWindow().GetSize();
		mCamera = EditorCamera(75.f, size.x / (float)size.y, .01f, 1000.f);

		RenderCommand::ClearColor(.3f, .3f, .3f);
	}

	void ReflectionLayer::OnDetach()
	{
	}

	void ReflectionLayer::OnUpdate(float dt)
	{
		mCamera.ProcessInput();

		RenderCommand::Clear();

		mReflectionShader->Bind();

		Renderer::Begin();

		// capture reflections
		{
			for (int i = 0; i < 6; i++)
			{
				mRelfectionTarget->Bind(i);
				mReflectionShader->SetUniform("uCaptureReflections", true);

				DrawScene();

				mRelfectionTarget->UnBind();
			}
		}

		auto size = Application::Get().GetWindow().GetSize();
		RenderCommand::SetViewport(0, 0, size.x, size.y);

		Renderer::SubmitCamera(mCamera.GetProjection(), mCamera.GetView());

		mRelfectionTarget->GetTargetTexture()->Bind();
		mReflectionShader->SetUniform("uCaptureReflections", false);

		DrawScene();

		Renderer::End();
	}
	void ReflectionLayer::OnEvent(Event &e)
	{
		mCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch(this, &ReflectionLayer::OnWindowResize);
	}

	bool ReflectionLayer::OnWindowResize(WindowResizeEvent &e)
	{
		RenderCommand::SetViewport(0, 0, e.x, e.y);
		mCamera.Resize(e.x, e.y);
		return false;
	}
	void ReflectionLayer::DrawScene()
	{
		static float radius = 9.0f;
		float x = glm::cos(Time::Get()) * radius;
		float y = glm::sin(Time::Get()) * radius;

		ReflectionMaterial material{};

		material.Color = {1, 0, 0, 1};
		material.IOR = 1.33f;
		material.Reflective = 0.5f;
		mReflectUBO->SetData(&material, sizeof(ReflectionMaterial));

		mPlaneIndirect->Draw(FTransform({0, -4, 0}, {0, 0, 0}));

		material.Color = {1, 0, 1, 1};
		material.IOR = 1.52f;
		material.Reflective = 0.5f;
		mReflectUBO->SetData(&material, sizeof(ReflectionMaterial));

		mSphereIndirect->Draw(FTransform({x, 0, y}));

		material.Color = {1, .5f, 0, .5f};
		material.IOR = 2.42f;
		material.Reflective = 1.f;
		mReflectUBO->SetData(&material, sizeof(ReflectionMaterial));

		mSphereIndirect->Draw(FTransform({-5, 0, 0}));
	}
} // namespace BHive