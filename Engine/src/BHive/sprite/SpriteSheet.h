#pragma once

#include "asset/Asset.h"
#include "math/Math.h"
#include "sprite/Sprite.h"

namespace BHive
{
	class Sprite;
	class Texture2D;

	typedef std::vector<Sprite> Sprites;

	REFLECT_STRUCT()
	struct FSpriteSheetGrid
	{
		REFLECT_PROPERTY()
		uint32_t mRows = 0;

		REFLECT_PROPERTY()
		uint32_t mColumns = 0;

		REFLECT_PROPERTY()
		glm::vec2 mCellSize{0.0f};

		template <typename A>
		void Serialize(A &ar)
		{
			ar(mRows, mColumns, mCellSize);
		}
	};

	REFLECT_STRUCT()
	class SpriteSheet : public Asset
	{

	public:
		REFLECT_CONSTRUCTOR()
		SpriteSheet() = default;

		SpriteSheet(const Ref<Texture2D> &source, const FSpriteSheetGrid &grid);

		void CreateSprites();

		REFLECT_FUNCTION()
		void SetSource(const Ref<Texture2D> &source);

		REFLECT_FUNCTION()
		void SetGrid(const FSpriteSheetGrid &grid);

		REFLECT_FUNCTION()
		const Ref<Texture2D> &GetSource() const { return mSource; }
		const Sprites &GetSprites() const { return mSprites; }

		REFLECT_FUNCTION()
		const FSpriteSheetGrid &GetGrid() const { return mGrid; }

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(Asset)

	private:
		Sprites mSprites;

		REFLECT_PROPERTY(Setter = SetSource, Getter = GetSource)
		Ref<Texture2D> mSource;

		REFLECT_PROPERTY(Setter = SetGrid, Getter = GetGrid)
		FSpriteSheetGrid mGrid;
	};

} // namespace BHive