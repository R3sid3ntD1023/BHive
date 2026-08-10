#pragma once

#include "core/Core.h"
#include "core/Layer.h"
#include "gfx/cameras/EditorCamera.h"
#include "core/events/ApplicationEvents.h"
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

		void OnEvent(Event &e) override;

	private:
		bool OnWindowResize(WindowResizeEvent &e);

	private:
		Ref<SceneRenderer> mSceneRenderer;
		Ref<BaseMesh> mMesh;
		Ref<BaseMesh> mPlane;
		EditorCamera mCamera;
		glm::uvec2 mViewportSize{0, 0};
		bool mViewportActive = false;

		MaterialTable mMaterialTables[2];
	};
} // namespace BHive