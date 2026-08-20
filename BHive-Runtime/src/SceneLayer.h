#pragma once

#include "core/Core.h"
#include "core/Layer.h"
#include "gfx/cameras/EditorCamera.h"
#include "core/events/ApplicationEvents.h"
#include "gfx/material/MaterialTable.h"
#include "gfx/cameras/EditorCameraController.h"

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

	private:
		Ref<SceneRenderer> mSceneRenderer;
		Ref<BaseMesh> mMesh;
		Ref<BaseMesh> mPlane;
		MaterialTable mMaterialTables[3];
		EditorCamera mCamera;
		glm::uvec2 mViewportSize{0, 0};
		bool mViewportActive = false;
		EditorCameraController mCameraController;
	};

} // namespace BHive