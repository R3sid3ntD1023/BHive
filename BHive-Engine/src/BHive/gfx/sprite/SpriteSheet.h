#pragma once

#include "asset/Asset.h"
#include "gfx/registries/Handles.h"

namespace BHive
{
	class Sprite;
	class Texture2D;

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

		SpriteSheet(TexturePtr source, const FSpriteSheetGrid &grid);

		void CreateSprites();

		void SetSource(TexturePtr source);

		void SetGrid(const FSpriteSheetGrid &grid);

		TexturePtr GetSource() const { return mSource; }

		const auto &GetSprites() const { return mSprites; }

		const FSpriteSheetGrid &GetGrid() const { return mGrid; }

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(Asset)

	private:
		std::vector<SpritePtr> mSprites;

		TexturePtr mSource;

		FSpriteSheetGrid mGrid;
	};

	REFLECT_EXTERN(SpriteSheet)
} // namespace BHive