#pragma once

#include "TAssetEditor.h"

namespace BHive
{
	class TextureEditor : public TAssetEditor<Texture2D>
	{
	protected:
		virtual void OnWindowRender();
	};
}