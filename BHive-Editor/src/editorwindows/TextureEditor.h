#pragma once

#include "TAssetEditor.h"
#include "gfx/Texture.h"

namespace BHive
{
	class TextureEditor : public TAssetEditor<Texture2D>
	{
	protected:
		virtual void OnUpdateContent() override;

		virtual void OnSetContext(const Ref<Texture2D> &asset) override;

	private:
		FSubTexture mSubTexture;
	};

} // namespace BHive