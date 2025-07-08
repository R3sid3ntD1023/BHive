#pragma once

#include "gfx/cameras/EditorCamera.h"
#include "mesh/BaseMesh.h"
#include "TAssetEditor.h"

namespace BHive
{
	class Framebuffer;

	class MeshEditor : public TAssetEditor<BaseMesh>
	{
	public:
		MeshEditor();

		virtual void OnWindowRender() override;
		virtual void OnEvent(Event &event) override;

	private:
		EditorCamera mCamera;
		glm::vec2 mViewportSize{300, 300};
		glm::vec2 mViewportPanelSize{300, 300};
		Ref<class SceneRenderer> mSceneRenderer;
	};
} // namespace BHive