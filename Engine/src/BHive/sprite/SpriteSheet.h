#pragma once

#include "asset/Asset.h"
#include "math/Math.h"
#include "sprite/Sprite.h"

namespace BHive
{
	class Sprite;
	class Texture2D;

	typedef std::vector<Sprite> Sprites;

	DECLARE_STRUCT()
	struct FSpriteSheetGrid
	{
		DECLARE_PROPERTY()
		uint32_t mRows = 0;

		DECLARE_PROPERTY()
		uint32_t mColumns = 0;

		DECLARE_PROPERTY()
		glm::vec2 mCellSize{0.0f};

		template <typename A>
		void Serialize(A &ar)
		{
			ar(mRows, mColumns, mCellSize);
		}
	};

	DECLARE_STRUCT()
	class SpriteSheet : public Asset
	{

	public:
		DECLARE_CONSTRUCTOR()
		SpriteSheet() = default;

		SpriteSheet(const Ref<Texture2D> &source, const FSpriteSheetGrid &grid);

		void CreateSprites();

		DECLARE_FUNCTION()
		void SetSource(const Ref<Texture2D>& source);

		DECLARE_FUNCTION()
		void SetGrid(const FSpriteSheetGrid &grid);

		DECLARE_FUNCTION()
		const Ref<Texture2D> &GetSource() const { return mSource; }

		const Sprites &GetSprites() const { return mSprites; }

		DECLARE_FUNCTION()
		const FSpriteSheetGrid &GetGrid() const { return mGrid; }

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLE_CLASS(Asset)

	private:
		Sprites mSprites;

		DECLARE_PROPERTY(Setter = SetSource, Getter = GetSource)
		Ref<Texture2D> mSource;

		DECLARE_PROPERTY(Setter = SetGrid, Getter = GetGrid)
		FSpriteSheetGrid mGrid;
	};

} // namespace BHive