#pragma once

#include "cameras/EditorCamera.h"
#include "mesh/IRenderableAsset.h"
#include "TAssetEditor.h"

namespace BHive
{
	class MeshEditor : public TAssetEditor<BaseMesh>
	{
	public:
		MeshEditor();

		virtual void OnWindowRender() override;
		virtual void OnEvent(Event &event) override;

	private:
		EditorCamera mCamera;
		Ref<class SceneRenderer> mRenderer;
		glm::vec2 mViewportSize{300, 300};
		glm::vec2 mViewportPanelSize{300, 300};
	};
} // namespace BHive