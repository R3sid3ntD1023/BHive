#pragma once

#include "core/Core.h"
#include "core/Layer.h"
#include "core/events/ApplicationEvents.h"
#include "gfx/cameras/EditorCamera.h"
#include "gfx/cameras/EditorCameraController.h"
#include "gfx/material/MaterialTable.h"
#include "gfx/renderers/SceneRenderer.h"

namespace BHive
{
	class BaseMesh;
	class Texture2D;
	class Font;

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

		Ref<BaseMesh> mSphere;
		Ref<BaseMesh> mPlane;
		Ref<BaseMesh> mMesh;
		Ref<Texture2D> mTexture;
		Ref<Font> mFont;

		glm::uvec2 mViewportSize{0, 0};
		bool mViewportActive = false;

		Ref<SceneRenderer> mSceneRenderer;

		FTransform mObjectTransforms[5];

		MaterialTable mMaterials[4];

		EditorCameraController mCameraController;
	};
} // namespace BHive