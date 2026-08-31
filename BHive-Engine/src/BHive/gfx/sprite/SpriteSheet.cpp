#include "gfx/Texture.h"
#include "Sprite.h"
#include "SpriteSheet.h"
#include "gfx/factories/SpriteFactory.h"

namespace BHive
{

	SpriteSheet::SpriteSheet(TexturePtr source, const FSpriteSheetGrid &grid)
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

		for (uint32_t c = 0; c < mGrid.Columns; c++)
		{
			for (uint32_t r = 0; r < mGrid.Rows; r++)
			{
				FSpriteGenerator generator{{c, r}, mGrid.CellSize, {1, 1}};
				SpritePtr sprite = SpriteFactory::Create(mSource, generator);
				mSprites.emplace_back(sprite);
			}
		}
	}

	void SpriteSheet::SetSource(TexturePtr source)
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
		ar(mSource, mGrid, mSprites);
	}

	void SpriteSheet::Load(cereal::BinaryInputArchive &ar)
	{
		Asset::Load(ar);
		ar(mSource, mGrid, mSprites);

		CreateSprites();
	}

	REFLECT(SpriteSheet)
	{
		{
			BEGIN_REFLECT(FSpriteSheetGrid) REFLECT_PROPERTY(Rows) REFLECT_PROPERTY(Columns) REFLECT_PROPERTY(CellSize);
		}
		{
			BEGIN_REFLECT(SpriteSheet)
			REFLECT_CONSTRUCTOR()
			REFLECT_PROPERTY("Source", GetSource, SetSource)
			REFLECT_PROPERTY("Grid", GetGrid, SetGrid) /* REFLECT_PROPERTY("Sprites", mSprites)*/;
		}

		rttr::type::register_wrapper_converter_for_base_classes<Ref<SpriteSheet>>();
	}

} // namespace BHive