#pragma once

#include "math/Math.h"
#include "sprite/Sprite.h"
#include "asset/Asset.h"

namespace BHive
{
	class Sprite;
	class Texture2D;

	typedef std::vector<Sprite> Sprites;

	struct FSpriteSheetGrid
	{
		uint32_t mRows = 1;
		uint32_t mColumns = 1;
		glm::vec2 mCellSize{0.0f};
	};

	class SpriteSheet : public Asset
	{

	public:
		SpriteSheet() = default;
		SpriteSheet(const TAssetHandle<Texture2D> &source, const FSpriteSheetGrid &grid);

		void CreateSprites();

		void SetSource(const TAssetHandle<Texture2D> &source);
		void SetGrid(const FSpriteSheetGrid &grid);

		const TAssetHandle<Texture2D> &GetSource() const { return mSource; }
		const Sprites &GetSprites() const { return mSprites; }
		const FSpriteSheetGrid &GetGrid() const { return mGrid; }

		virtual void Save(cereal::JSONOutputArchive &ar) const override;

		virtual void Load(cereal::JSONInputArchive &ar) override;

	private:
		Sprites mSprites;
		TAssetHandle<Texture2D> mSource;
		FSpriteSheetGrid mGrid;

		REFLECTABLEV(Asset)

		friend class SpriteSheetSerializer;
	};

	template <typename A>
	void Serialize(A &ar, FSpriteSheetGrid &obj)
	{
		ar(MAKE_NVP("Rows", obj.mRows), MAKE_NVP("Columns", obj.mColumns),
		   MAKE_NVP("CellSize", obj.mCellSize));
	}

	REFLECT_EXTERN(FSpriteSheetGrid)
	REFLECT_EXTERN(SpriteSheet)

} // namespace BHive