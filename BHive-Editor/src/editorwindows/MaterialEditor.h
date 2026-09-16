#pragma once

#include "TAssetEditor.h"
#include "gfx/cameras/EditorCamera.h"
#include "gfx/material/Material.h"

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

		void OnSetContext(MaterialPtr material) override;

		/*bool OnSave(const std::filesystem::path &path) override;*/

		/*Ref<Material> GetEditedAssetOverride() const override;*/

	private:
		EditorCamera mCamera;
		MeshPtr mSphere;
		MaterialPtr mMaterial;
		glm::vec2 mViewportSize{300, 300};
		glm::vec2 mViewportPanelSize{300, 300};

		Ref<SceneRenderer> mSceneRenderer;
		/*Ref<Material> mTempMaterialAsset;*/
	};
} // namespace BHive
