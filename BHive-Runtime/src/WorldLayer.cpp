#include "WorldLayer.h"
#include "core/FPSCounter.h"
#include "gfx/renderers/SceneRenderer.h"
#include "runtime/World.h"

namespace BHive
{
	void WorldLayer::OnAttach(Application &app)
	{
		const auto &window = app.GetWindow();
		const auto &size = window.GetSize();
		const auto &aspect = window.GetAspectRatio();

		CameraSensitivity sensitivity{};
		sensitivity.ZoomSpeed = EngineConfig::ZoomSpeed;
		sensitivity.PanSpeed = EngineConfig::PanSpeed;
		sensitivity.MoveSpeed = EngineConfig::MoveSpeed;
		sensitivity.OrbitSpeed = EngineConfig::OrbitSpeed;

		mCamera = EditorCamera(75.0f, aspect, 0.1f, 1000.0f);
		mCamera.SetStartState({0, 10, 10}, -90.f, -35.0f);
		mCamera.SetSensitivity(sensitivity);

		mCameraController.SetCamera(&mCamera);

		mRenderer = CreateRef<SceneRenderer>();
		mRenderer->Init(size);

		mCurrentWorld = CreateRef<World>();
		mCurrentWorld->Begin();

		mSceneOutput = MaterialFactory::Create("Scene.glsl");
		mSceneOutputPipeline = PipelineFactory::Create(Pipeline::GetDefaultGraphicsPipelineState());
	}

	void WorldLayer::OnUpdate(float dt)
	{
		mCameraController.Update(dt);

		mCurrentWorld->Update(dt);
	}

	void WorldLayer::OnRender(Renderer &renderer)
	{
		mRenderer->Begin(mCamera.GetProjection(), mCamera.GetView());

		mCurrentWorld->Render(mRenderer.get(), &renderer);

		renderer.Line.DrawGrid({});

		mRenderer->End();

		mSceneOutput.As<Material>()->SetTexture("Scene", {mRenderer->GetOutput()});

		auto &pass = renderer.BeginPass("RenderToScreen", EPassType::Present);
		pass.BeginPhase(EPhaseType::Graphics);
		pass.UseTexture(mRenderer->GetOutput(), EImageUsage::ColorRead);
		pass.Emplace<CmdBindPipeline>()(mSceneOutputPipeline);
		pass.Emplace<CmdBindMaterial>()(mSceneOutput);
		pass.Emplace<CmdDrawFullScreen>();
		pass.EndPhase();
		renderer.EndPass();
	}

	void WorldLayer::OnGuiRender()
	{
	}

	void WorldLayer::OnEvent(Event &e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch(this, &WorldLayer::OnWindowResize);
	}

	void WorldLayer::OnDetach()
	{
		mCurrentWorld->End();
	}

	bool WorldLayer::OnWindowResize(WindowResizeEvent &e)
	{
		mRenderer->Resize({e.x, e.y});
		mCurrentWorld->Resize(e.x, e.y);
		return false;
	}
} // namespace BHive