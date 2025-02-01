#pragma once

#include "TAssetEditor.h"
#include "prefab/Prefab.h"

namespace BHive
{
	struct PrefabEditor : public TAssetEditor<Prefab>
	{
	protected:
		virtual void OnWindowRender();
	};
} // namespace BHive