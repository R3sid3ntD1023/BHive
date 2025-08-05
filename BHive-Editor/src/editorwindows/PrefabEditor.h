#pragma once

#include "TAssetEditor.h"
#include "prefab/Prefab.h"
#include "windows/SceneHeirarchyWindow.h"

namespace BHive
{
	struct PrefabEditor : public TAssetEditor<Prefab>
	{
	protected:
		virtual void OnUpdateContent() override;
		virtual void OnSetContext(const Ref<Prefab> &asset) override;

	private:
		ImSceneHierarchy mSceneHierarchy;
	};
} // namespace BHive