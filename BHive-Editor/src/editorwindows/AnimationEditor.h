#pragma once

#include "mesh/SkeletalAnimation.h"
#include "TAssetEditor.h"

namespace BHive
{
	class AnimationEditor : public TAssetEditor<SkeletalAnimation>
	{
	public:
		virtual void OnUpdateContent() override;
	};
} // namespace BHive