#pragma once

#include "TAssetEditor.h"
#include "prefab/Prefab.h"
#include "SceneHeirarchyPanel.h"
#include "PropertiesPanel.h"
#include "DetailsPanel.h"
#include "selection/Selection.h"

namespace BHive
{
	struct PrefabEditor : public TAssetEditor<Prefab>
	{
		PrefabEditor();

	protected:
		virtual void OnWindowRender();
		virtual void OnSetContext(const Ref<Asset> &asset) override;

	private:
		SceneHierarchyPanel mSceneHierarchy;
		PropertiesPanel mProperties;
		DetailsPanel mDetails;
		Selection mSelection;
	};
} // namespace BHive