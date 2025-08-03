#pragma once

#include "TAssetEditor.h"
#include "prefab/Prefab.h"
#include "SceneHeirarchyPanel.h"

namespace BHive
{
	struct PrefabEditor : public TAssetEditor<Prefab>
	{
	protected:
		virtual void OnWindowRender();
		virtual void OnSetContext(const Ref<Prefab> &asset) override;

	private:
		SceneHierarchyPanel mSceneHierarchy;
	};
} // namespace BHive