#include "SpriteFactory.h"

namespace BHive
{
	SpritePtr SpriteFactory::Create(TexturePtr texture, const FSpriteGenerator &generator)
	{
		return CreateResource<Sprite>(texture, generator);
	}

	SpritePtr SpriteFactory::Create(TexturePtr texture, const glm::vec2 &min, const glm::vec2 &max)
	{
		return CreateResource<Sprite>(texture, min, max);
	}

	FlipBookPtr FlipBookFactory::Create()
	{
		return CreateResource<FlipBook>();
	}

	FlipBookPtr FlipBookFactory::Create(const std::vector<FlipBook::Frame> &frames)
	{
		return CreateResource<FlipBook>(frames);
	}

	SpriteSheetPtr SpriteSheetFactory::Create()
	{
		return CreateResource<SpriteSheet>();
	}

	SpriteSheetPtr SpriteSheetFactory::Create(TexturePtr source, const FSpriteSheetGrid &grid)
	{
		return CreateResource<SpriteSheet>(source, grid);
	}

	SpriteSheetPtr SpriteSheetFactory::Create(TexturePtr source, uint32_t rows, uint32_t columns, const glm::vec2 &cellSize)
	{
		FSpriteSheetGrid grid{};
		grid.Rows = rows;
		grid.Columns = columns;
		grid.CellSize = cellSize;
		return CreateResource<SpriteSheet>(source, grid);
	}

} // namespace BHive