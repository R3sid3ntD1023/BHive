#include "gfx/Texture.h"
#include "Sprite.h"
#include "SpriteSheet.h"

namespace BHive
{

	SpriteSheet::SpriteSheet(const TAssetHandle<Texture2D> &source, const FSpriteSheetGrid &grid)
		: mSource(source),
		  mGrid(grid)
	{
		CreateSprites();
	}

	void SpriteSheet::CreateSprites()
	{
		mSprites.clear();

		for (uint32_t c = 0; c < mGrid.mColumns; c++)
		{
			for (uint32_t r = 0; r < mGrid.mRows; r++)
			{
				Sprite sprite = Sprite(mSource, {c, r}, mGrid.mCellSize, {1, 1});
				mSprites.emplace_back(sprite);
			}
		}
	}

	void SpriteSheet::SetSource(const TAssetHandle<Texture2D> &source)
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
		// ar(mSource, mGrid, mSprites);
	}

	void SpriteSheet::Load(cereal::BinaryInputArchive &ar)
	{
		Asset::Load(ar);
		// ar(mSource, mGrid, mSprites);

		CreateSprites();
	}

	REFLECT(FSpriteSheetGrid)
	{
		BEGIN_REFLECT(FSpriteSheetGrid)
		REFLECT_PROPERTY("Rows", mRows)
		(META_DATA(EPropertyMetaData_Min, 0)) REFLECT_PROPERTY("Columns", mColumns)(META_DATA(EPropertyMetaData_Min, 0))
			REFLECT_PROPERTY("CellSize", mCellSize);
	}

	REFLECT(SpriteSheet)
	{
		BEGIN_REFLECT(SpriteSheet)
		REFLECT_CONSTRUCTOR()
		REFLECT_PROPERTY("Source", GetSource, SetSource)
		REFLECT_PROPERTY("Grid", GetGrid, SetGrid);
	}

} // namespace BHive