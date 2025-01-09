#pragma once

#include "TAssetEditor.h"
#include "sprite/Sprite.h"
#include "sprite/SpriteSheet.h"

namespace BHive
{
	class SpriteEditor : public TAssetEditor<Sprite>
	{

	protected:
		virtual void OnWindowRender();
	};

	class SpriteSheetEditor : public TAssetEditor<SpriteSheet>
	{
	protected:
		virtual void OnWindowRender();


	private:
		void ExtractSprites(const std::filesystem::path &directory, const std::string &filename, const std::string &ext);

	private:
		float mScale = .75f;
	};
}