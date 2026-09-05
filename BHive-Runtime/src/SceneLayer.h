#pragma once

#include "core/Core.h"
#include "core/Layer.h"
#include "core/events/ApplicationEvents.h"
#include "gfx/cameras/EditorCamera.h"
#include "gfx/cameras/EditorCameraController.h"
#include "gfx/material/MaterialTable.h"

namespace BHive
{
	class Texture2D;
	class BaseMesh;
	class SceneRenderer;

	class SceneLayer : public Layer
	{
	public:
		void OnAttach(Application &app) override;

		void OnDetach() override;

		void OnUpdate(float) override;

		void OnRender(Renderer &renderer) override;

		void OnGuiRender() override;

		void OnEvent(Event &e);

		bool OnKeyEvent(KeyEvent &e);

	private:
		Ref<SceneRenderer> mSceneRenderer;
		MeshPtr mMesh;
		MeshPtr mPlane;
		MaterialTable mMaterialTables[3];
		EditorCamera mCameras[2];
		glm::uvec2 mViewportSize{0, 0};
		bool mViewportActive = false;
		EditorCameraController mCameraController;
		uint32_t mCurrentCameraIndex = 0;
		MaterialPtr mStandardMaterial;
	};

} // namespace BHive