#pragma once

#include "mesh/SkeletalAnimation.h"
#include "TAssetEditor.h"

namespace BHive
{
	class AnimationEditor : public TAssetEditor<SkeletalAnimation>
	{
	public:
		virtual void OnWindowRender();
	};
} // namespace BHive