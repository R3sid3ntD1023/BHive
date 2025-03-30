#include "gfx/Texture.h"
#include "Sprite.h"
#include "SpriteSheet.h"

namespace BHive
{

	SpriteSheet::SpriteSheet(const Ref<Texture2D> &source, const FSpriteSheetGrid &grid)
		: mSource(source),
		  mGrid(grid)
	{
		CreateSprites();
	}

	void SpriteSheet::CreateSprites()
	{
		if (!mSource)
			return;

		mSprites.clear();

		for (uint32_t c = 0; c < mGrid.mColumns; c++)
		{
			for (uint32_t r = 0; r < mGrid.mRows; r++)
			{
				FSpriteGenerator generator{{c, r}, mGrid.mCellSize, {1, 1}};
				Sprite sprite = Sprite(mSource, generator);
				mSprites.emplace_back(sprite);
			}
		}
	}

	void SpriteSheet::SetSource(const Ref<Texture2D> &source)
	{
		mSource = source;
		CreateSprites();
	}

	void SpriteSheet::SetGrid(const FSpriteSheetGrid &grid)
	{
		mGrid = grid;
		CreateSprites();
	}

	void SpriteSheet::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);
		ar(TAssetHandle<Texture2D>(mSource), mGrid, mSprites);
	}

	void SpriteSheet::Load(cereal::BinaryInputArchive &ar)
	{
		Asset::Load(ar);
		ar(TAssetHandle<Texture2D>(mSource), mGrid, mSprites);

		CreateSprites();
	}

} // namespace BHive