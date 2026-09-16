#pragma once

#include "TAssetEditor.h"
#include "gfx/animation/SkeletalAnimation.h"

namespace BHive
{
	class AnimationEditor : public TAssetEditor<SkeletalAnimation>
	{
	public:
		virtual void OnUpdateContent() override;
	};
} // namespace BHive