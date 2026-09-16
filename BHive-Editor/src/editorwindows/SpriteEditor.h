#pragma once

#include "TAssetEditor.h"
#include "gfx/sprite/Sprite.h"

namespace BHive
{
	class SpriteEditor : public TAssetEditor<Sprite>
	{

	protected:
		virtual void OnUpdateContent() override;
	};

} // namespace BHive