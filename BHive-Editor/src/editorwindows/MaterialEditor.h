#pragma once

#include "gfx/cameras/EditorCamera.h"
#include "material/Material.h"
#include "TAssetEditor.h"

namespace BHive
{
	class Framebuffer;
	class SceneRenderer;

	class MaterialEditor : public TAssetEditor<Material>
	{
	public:
		MaterialEditor();

	protected:
		virtual void OnWindowRender();

		void OnSetContext(const Ref<Material> &asset) override;

	private:
		EditorCamera mCamera;
		Ref<Framebuffer> mFramebuffer;
		static inline Ref<class StaticMesh> mSphere;
		glm::vec2 mViewportSize{300, 300};
		glm::vec2 mViewportPanelSize{300, 300};

		Ref<SceneRenderer> mSceneRenderer;
	};
} // namespace BHive
