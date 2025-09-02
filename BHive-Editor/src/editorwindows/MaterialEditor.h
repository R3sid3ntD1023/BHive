#pragma once

#include "gfx/cameras/EditorCamera.h"
#include "material/Material.h"
#include "TAssetEditor.h"

namespace BHive
{
	class SceneRenderer;
	class Material;

	class MaterialEditor : public TAssetEditor<Material>
	{
	public:
		MaterialEditor();

	protected:
		virtual void OnUpdateContent() override;

		void OnSetContext(const Ref<Material> &asset) override;

		/*bool OnSave(const std::filesystem::path &path) override;*/

		/*Ref<Material> GetEditedAssetOverride() const override;*/

	private:
		EditorCamera mCamera;
		static inline Ref<class StaticMesh> mSphere;
		glm::vec2 mViewportSize{300, 300};
		glm::vec2 mViewportPanelSize{300, 300};

		Ref<SceneRenderer> mSceneRenderer;
		/*Ref<Material> mTempMaterialAsset;*/
	};
} // namespace BHive
