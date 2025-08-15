#pragma once

#include "sprite/SpriteSheet.h"
#include "TAssetEditor.h"

namespace BHive
{
	class SpriteSheetEditor : public TAssetEditor<SpriteSheet>
	{
	protected:
		virtual void OnUpdateContent() override;

	private:
		void DrawSprites();
		void ExtractSprites(const std::filesystem::path &path);

	private:
		float mScale = .75f;
	};
} // namespace BHive
