#pragma once

#include "asset/Asset.h"
#include "math/Math.h"
#include "sprite/Sprite.h"

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

	class SpriteSheet : public Asset, public ISerializable
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

		void Serialize(StreamWriter& ar) const;
		void Deserialize(StreamReader& ar);

	private:
		Sprites mSprites;
		TAssetHandle<Texture2D> mSource;
		FSpriteSheetGrid mGrid;

		ASSET_CLASS(SpriteSheet);
		REFLECTABLEV(Asset)

		friend class SpriteSheetSerializer;
	};

	void Serialize(StreamWriter& ar, const FSpriteSheetGrid& obj);
	void Deserialize(StreamReader& ar, FSpriteSheetGrid& obj);

	REFLECT_EXTERN(FSpriteSheetGrid)
	REFLECT_EXTERN(SpriteSheet)

}