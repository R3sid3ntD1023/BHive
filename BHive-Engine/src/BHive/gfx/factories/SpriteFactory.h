#pragma once

#include "gfx/sprite/FlipBook.h"
#include "gfx/sprite/Sprite.h"
#include "gfx/sprite/SpriteSheet.h"
#include "IResourceFactory.h"

namespace BHive
{
	// struct BHIVE_API SpriteFactory : public IResourceFactory<Sprite>
	// {
	// 	static SpritePtr Create(TexturePtr texture, const FSpriteGenerator &generator);

	// 	static SpritePtr Create(TexturePtr texture, const glm::vec2 &min, const glm::vec2 &max);
	// };

	struct BHIVE_API FlipBookFactory : public IResourceFactory<FlipBook>
	{
		static FlipBookPtr Create();

		static FlipBookPtr Create(const std::vector<FlipBook::Frame> &frames);
	};

	struct BHIVE_API SpriteSheetFactory : public IResourceFactory<SpriteSheet>
	{
		static SpriteSheetPtr Create();

		static SpriteSheetPtr Create(Ref<Texture2D> source, const FSpriteSheetGrid &grid);

		static SpriteSheetPtr Create(Ref<Texture2D> source, uint32_t rows, uint32_t columns, const glm::vec2 &cellSize);
	};
} // namespace BHive