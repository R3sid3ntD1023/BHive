#pragma once

#include "gfx/Camera.h"
#include "material/Material.h"
#include "TAssetEditor.h"

namespace BHive
{
	class MaterialEditor : public TAssetEditor<Material>
	{
	public:
		MaterialEditor();

	protected:
		virtual void OnWindowRender();

	private:
		Camera mCamera;
		Ref<class SceneRenderer> mRenderer;
		static inline Ref<class StaticMesh> mSphere;
		glm::vec2 mViewportSize{300, 300};
		glm::vec2 mViewportPanelSize{300, 300};
	};
} // namespace BHive
