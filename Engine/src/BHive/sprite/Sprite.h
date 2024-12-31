#pragma once

#include "core/Core.h"
#include "math/Math.h"
#include "asset/Asset.h"
#include "gfx/Texture.h"
#include "serialization/Serialization.h"

namespace BHive
{
	class Texture2D;

	class Sprite : public Asset, public ISerializable
	{
	public:
		Sprite() = default;
		Sprite(const Sprite &other);

		Sprite(const TAssetHandle<Texture2D> &texture, const glm::vec2 &coords,
			   const glm::vec2 &cellSize, const glm::vec2 &spriteSize);

		Sprite(const TAssetHandle<Texture2D> &texture, const glm::vec2 &min, const glm::vec2 &max);

		void SetCoords(const glm::vec2 &min, const glm::vec2 &max);
		void SetCoords(const glm::vec2 &coords, const glm::vec2 &cellSize, const glm::vec2 &spriteSize);

		const glm::vec2 &GetCoordinates() const;
		const glm::vec2 &GetCellSize() const;
		const glm::vec2 &GetSpriteSize() const;

		void SetCoordinates(const glm::vec2 &coordinates);
		void SetCellSize(const glm::vec2 &cellSize);
		void SetSpriteSize(const glm::vec2 &spriteSize);

		const glm::vec2 &GetMinCoords() const;
		const glm::vec2 &GetMaxCoords() const;

		void SetSourceTexture(const TAssetHandle<Texture2D> &texture);
		const TAssetHandle<Texture2D> &GetSourceTexture() const { return mSource; }
		const glm::vec2 *GetCoords() const { return mCoords.data(); }

		void Serialize(StreamWriter& ar) const;
		void Deserialize(StreamReader& ar);

		ASSET_CLASS(Sprite);
		REFLECTABLEV(Asset)

	private:
		TAssetHandle<Texture2D> mSource;
		std::array<glm::vec2, 4> mCoords = {};

		glm::vec2 mCoordinates{ 0,0 };
		glm::vec2 mCellSize{ 0,0 };
		glm::vec2 mSpriteSize{ 0, 0 };
	};

	REFLECT_EXTERN(Sprite)

}
