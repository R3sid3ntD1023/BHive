#pragma once

#include "gfx/cameras/EditorCamera.h"
#include "mesh/BaseMesh.h"
#include "TAssetEditor.h"

namespace BHive
{
	class MeshEditor : public TAssetEditor<BaseMesh>
	{
	public:
		MeshEditor();

		virtual void OnUpdateContent() override;

		void OnEvent(Event &event);

	private:
		EditorCamera mCamera;
		glm::vec2 mViewportSize{300, 300};
		glm::vec2 mViewportPanelSize{300, 300};
		Ref<class SceneRenderer> mSceneRenderer;
	};
} // namespace BHive