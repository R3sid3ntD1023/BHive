#include "WorldLayer.h"
#include "audio/AudioImporter.h"
#include "audio/AudioSource.h"
#include "core/FPSCounter.h"
#include "gfx/renderers/SceneRenderer.h"
#include "runtime/Components.h"
#include "runtime/GameObject.h"
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

		mSceneOutput = MaterialFactory::Create("Scene.glsl");
		mSceneOutputPipeline = PipelineFactory::Create(Pipeline::GetDefaultGraphicsPipelineState());

		mFont = FontFactory::Create(ENGINE_PATH "/data/fonts/Roboto/Roboto-Regular.ttf", 16.f);

		AssetManager::SetAssetManager(&mAssetManager);

		// auto decoded = AudioImporter::Import("E://Files//Mods//Resident Evil 4  - The Mercenaries - Leon Theme.wav");

		// AudioSource audio(decoded.Data, decoded.Specification);

		// AssetFactory factory{};
		// factory.Export(&audio, "Assets/Resident Evil 4  - The Mercenaries - Leon Theme.asset");

				// auto handle = mAssetManager.ImportAsset("Assets/Resident Evil 4  - The Mercenaries - Leon Theme.asset", rttr::type::get<AudioSource>());

		auto handle = mAssetManager.GetHandle("Assets/Resident Evil 4  - The Mercenaries - Leon Theme.asset");

		AssetHandle<AudioSource>(handle)->SetLooping(true);

		{
			auto gameObject = mCurrentWorld->CreateGameObject("AudioGameObj");
			gameObject->AddComponent<PhysicsComponent>();
			gameObject->AddComponent<SphereColliderComponent>();
			gameObject->GetComponent<TransformComponent>()->Transform.Translation = {0.f, 1.f, 0.f};
			auto comp = gameObject->AddComponent<AudioComponent>();
			comp->AutoPlay = false;
			comp->Audio = AssetHandle<AudioSource>(handle);
		}

		{
			auto gameObject = mCurrentWorld->CreateGameObject("GameObject2");
			auto p = gameObject->AddComponent<PhysicsComponent>();
			p->Settings.InitialVelocity = {10.f, 0.f, 0.f};
			p->Settings.BodyType = EBodyType::Dynamic;
			gameObject->AddComponent<CapsuleColliderComponent>();
			gameObject->GetComponent<TransformComponent>()->Transform.Translation = {2.f, 3.f, 0.f};
		}

		{
			auto gameObject = mCurrentWorld->CreateGameObject("GameObject3");
			gameObject->AddComponent<PhysicsComponent>();
			gameObject->AddComponent<BoxColliderComponent>()->Extents = {50.f, .5f, 50.f};
		}

		mCurrentWorld->Begin();
	}

	void WorldLayer::OnUpdate(float dt)
	{
		mCameraController.Update(dt);

		mCurrentWorld->Update(dt);
	}

	void WorldLayer::OnRender(Renderer &renderer)
	{
		auto fps = FPSCounter::Get().GetFPS();

		mRenderer->Begin(mCamera.GetProjection(), mCamera.GetView());

		mCurrentWorld->Render(mRenderer.get(), &renderer);

		renderer.Line.DrawGrid({});
		renderer.Quad.DrawText(mFont, 1.0f, std::format("{}", fps));

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