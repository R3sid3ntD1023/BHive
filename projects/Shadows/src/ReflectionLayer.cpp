#include "ReflectionLayer.h"
#include "gfx/ShaderManager.h"
#include "renderers/Renderer.h"
#include "core/Application.h"
#include "gfx/RenderCommand.h"

namespace BHive
{
	void ReflectionLayer::OnAttach()
	{
		mReflectionShader = ShaderManager::Get().Load(RESOURCE_PATH "shaders/reflection.glsl");
		mPlane = CreateRef<PPlane>(20.f, 20.f);
		mSphere = CreateRef<PSphere>(2.f);

		mRelfectionTarget = CreateRef<RenderTarget2D>(1024, 100.f);

		mPlaneIndirect = CreateRef<IndirectRenderable>();
		mPlaneIndirect->Init(mPlane);

		mSphereIndirect = CreateRef<IndirectRenderable>();
		mSphereIndirect->Init(mSphere);

		auto size = Application::Get().GetWindow().GetSize();
		mCamera = EditorCamera(75.f, size.x / (float)size.y, .01f, 1000.f);

		RenderCommand::ClearColor(.3f, .3f, .3f);
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
		mReflectionShader->SetUniform("uColor", glm::vec3(1, 0, 0));

		mPlaneIndirect->Draw(FTransform({0, -4, 0}, {-90, 0, 0}));

		mReflectionShader->SetUniform("uColor", glm::vec3(1, .5, 0));
		mSphereIndirect->Draw(FTransform({5, 0, 0}));

		mReflectionShader->SetUniform("uColor", glm::vec3(1, .5, 1));
		mSphereIndirect->Draw(FTransform({-5, 0, 0}));
	}
} // namespace BHive