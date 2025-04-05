#pragma once

#include "TAssetEditor.h"
#include "gfx/textures/Texture2D.h"

namespace BHive
{
	class TextureEditor : public TAssetEditor<Texture2D>
	{
	protected:
		virtual void OnWindowRender();

		virtual void OnSetContext(const Ref<Texture2D> &asset) override;

	private:
		FSubTexture mSubTexture;
	};

} // namespace BHive