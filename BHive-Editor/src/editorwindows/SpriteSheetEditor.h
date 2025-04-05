#pragma once

#include "sprite/SpriteSheet.h"
#include "TAssetEditor.h"

namespace BHive
{
	class SpriteSheetEditor : public TAssetEditor<SpriteSheet>
	{
	protected:
		virtual void OnWindowRender();

	private:
		void DrawSprites();
		void ExtractSprites(const std::filesystem::path &directory, const std::string &filename, const std::string &ext);

	private:
		float mScale = .75f;
	};
} // namespace BHive
