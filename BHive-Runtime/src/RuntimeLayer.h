#pragma once

#include "core/Core.h"
#include "core/Layer.h"
#include "gfx/cameras/EditorCamera.h"
#include "core/events/ApplicationEvents.h"
#include "gfx/renderers/SceneRenderer.h"
#include "gfx/material/MaterialTable.h"
#include "gfx/cameras/EditorCameraController.h"

namespace BHive
{
	class BaseMesh;
	class Texture2D;

	class RuntimeLayer : public Layer
	{
	public:
		void OnAttach(Application &app) override;

		void OnDetach() override;

		void OnUpdate(float) override;

		void OnRender(Renderer &renderer) override;

		void OnGuiRender() override;

		void OnEvent(Event &e) override;

	private:
		bool OnWindowResize(WindowResizeEvent &e);

	private:
		EditorCamera mCamera;

		MeshPtr mSphere;
		MeshPtr mPlane;
		MeshPtr mMesh;
		Texture2DPtr mTexture;
		FontPtr mFont;

		glm::uvec2 mViewportSize{0, 0};
		bool mViewportActive = false;

		Ref<SceneRenderer> mSceneRenderer;

		FTransform mObjectTransforms[5];

		MaterialTable mMaterials[4];

		EditorCameraController mCameraController;
	};
} // namespace BHive