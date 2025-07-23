#pragma once

#include "asset/Asset.h"
#include "sprite/Sprite.h"

namespace BHive
{
	class Sprite;
	class Texture2D;

	typedef std::vector<Sprite> Sprites;

	struct FSpriteSheetGrid
	{
		uint32_t Rows = 0;

		uint32_t Columns = 0;

		glm::vec2 CellSize{0.0f};

		template <typename A>
		void Serialize(A &ar)
		{
			ar(Rows, Columns, CellSize);
		}
	};

	class BHIVE_API SpriteSheet : public Asset
	{

	public:
		SpriteSheet() = default;

		SpriteSheet(const Ref<Texture2D> &source, const FSpriteSheetGrid &grid);

		void CreateSprites();

		void SetSource(const Ref<Texture2D> &source);

		void SetGrid(const FSpriteSheetGrid &grid);

		const Ref<Texture2D> &GetSource() const { return mSource; }

		const Sprites &GetSprites() const { return mSprites; }

		const FSpriteSheetGrid &GetGrid() const { return mGrid; }

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(Asset)

	private:
		Sprites mSprites;

		Ref<Texture2D> mSource;

		FSpriteSheetGrid mGrid;
	};

	REFLECT_EXTERN(SpriteSheet)
} // namespace BHive