#include "SpriteSheet.h"
#include "Sprite.h"
#include "gfx/Texture.h"
#include "asset/AssetManager.h"

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

	void SpriteSheet::Serialize(StreamWriter &ar) const
	{
		Asset::Serialize(ar);
		ar(mSource, mGrid, mSprites);
	}

	void SpriteSheet::Deserialize(StreamReader &ar)
	{
		Asset::Deserialize(ar);
		ar(mSource, mGrid, mSprites);
		CreateSprites();
	}

	void Serialize(StreamWriter &ar, const FSpriteSheetGrid &obj)
	{
		ar(obj.mRows, obj.mColumns, obj.mCellSize);
	}

	void Deserialize(StreamReader &ar, FSpriteSheetGrid &obj)
	{
		ar(obj.mRows, obj.mColumns, obj.mCellSize);
	}

	REFLECT(FSpriteSheetGrid)
	{
		BEGIN_REFLECT(FSpriteSheetGrid)
		REFLECT_PROPERTY("Rows", mRows)
		(META_DATA(EPropertyMetaData_Min, 0))
			REFLECT_PROPERTY("Columns", mColumns)(META_DATA(EPropertyMetaData_Min, 0))
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