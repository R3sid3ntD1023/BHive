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

		virtual const char *GetFileDialogFilter() { return "Sprite (*.sprite)\0*.sprite\0"; };
	};

	class SpriteSheetEditor : public TAssetEditor<SpriteSheet>
	{
	protected:
		virtual void OnWindowRender();

		virtual const char *GetFileDialogFilter() { return "SpriteSheet (*.spritesheet)\0*.spritesheet\0"; };

	private:
		void ExtractSprites(const std::filesystem::path &directory, const std::string &filename, const std::string &ext);

	private:
		float mScale = .75f;
	};
}