#pragma once

#include "asset/EditorAssetManager.h"
#include "core/Application.h"
#include "core/Layer.h"
#include "gfx/cameras/EditorCamera.h"
#include "gfx/cameras/EditorCameraController.h"
#include "gfx/factories/GFXFactories.h"

namespace BHive
{
	class AudioSource;

	class WorldLayer : public Layer
	{
	public:
		void OnAttach(Application &app) override;

		void OnUpdate(float dt) override;

		void OnRender(Renderer &renderer) override;

		void OnGuiRender() override;

		void OnEvent(Event &e) override;

		void OnDetach() override;

	private:
		bool OnWindowResize(WindowResizeEvent &e);

	private:
		EditorCamera mCamera;
		EditorCameraController mCameraController;

		Ref<class World> mCurrentWorld;
		Ref<class SceneRenderer> mRenderer;

		PipelinePtr mSceneOutputPipeline;
		MaterialPtr mSceneOutput;
		FontPtr mFont;

		EditorAssetManager mAssetManager{"Assets"};
		struct AudioComponent *mAudioComponent;
	};
} // namespace BHive