#pragma once

#include "sprite/Sprite.h"
#include "TAssetEditor.h"

namespace BHive
{
	class SpriteEditor : public TAssetEditor<Sprite>
	{

	protected:
		virtual void OnUpdateContent() override;
	};

} // namespace BHive