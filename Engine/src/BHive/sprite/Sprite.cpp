#include "Sprite.h"
#include "gfx/Texture.h"
#include "asset/AssetManager.h"

namespace BHive
{
	Sprite::Sprite(const Sprite &other)
		: mSource(other.mSource), mCoords(other.mCoords), mCoordinates(other.mCoordinates), mCellSize(other.mCellSize),
		  mSpriteSize(other.mSpriteSize)
	{
	}

	Sprite::Sprite(const TAssetHandle<Texture2D> &texture, const glm::vec2 &coords, const glm::vec2 &cellSize,
				   const glm::vec2 &spriteSize)
		: mSource(texture), mCoordinates(coords), mCellSize(cellSize), mSpriteSize(spriteSize)
	{
		SetCoords(coords, cellSize, spriteSize);
	}

	Sprite::Sprite(const TAssetHandle<Texture2D> &texture, const glm::vec2 &min, const glm::vec2 &max)
		: mSource(texture)
	{
		SetCoords(min, max);
	}

	void Sprite::SetCoords(const glm::vec2 &min, const glm::vec2 &max)
	{
		auto texture = mSource;
		glm::vec2 texture_size = {texture->GetWidth(), texture->GetHeight()};

		auto size = max - min;
		auto min_coord = glm::min(size.x, size.y);
		glm::vec2 min_size = {min_coord, min_coord};

		mCellSize = min_size * texture_size;
		mCoordinates = (min * texture_size) / mCellSize;
		mSpriteSize = size / min_size;

		mCoords[0] = {min.x, min.y};
		mCoords[1] = {max.x, min.y};
		mCoords[2] = {max.x, max.y};
		mCoords[3] = {min.x, max.y};
	}

	void Sprite::SetCoords(const glm::vec2 &coords, const glm::vec2 &cellSize, const glm::vec2 &spriteSize)
	{
		mCoordinates = coords;
		mCellSize = cellSize;
		mSpriteSize = spriteSize;

		if (mSource)
		{
			auto texture = mSource;
			glm::vec2 texture_size = {texture->GetWidth(), texture->GetHeight()};
			glm::vec2 min = (coords * cellSize) / texture_size;
			glm::vec2 max = ((coords + spriteSize) * cellSize) / texture_size;

			mCoords[0] = {min.x, min.y};
			mCoords[1] = {max.x, min.y};
			mCoords[2] = {max.x, max.y};
			mCoords[3] = {min.x, max.y};
		}
	}

	const glm::vec2 &Sprite::GetCoordinates() const
	{
		return mCoordinates;
	}

	const glm::vec2 &Sprite::GetCellSize() const
	{
		return mCellSize;
	}

	const glm::vec2 &Sprite::GetSpriteSize() const
	{
		return mSpriteSize;
	}

	void Sprite::SetCoordinates(const glm::vec2 &coordinates)
	{
		mCoordinates = coordinates;
		SetCoords(mCoordinates, mCellSize, mSpriteSize);
	}

	void Sprite::SetCellSize(const glm::vec2 &cellSize)
	{
		mCellSize = cellSize;
		SetCoords(mCoordinates, mCellSize, mSpriteSize);
	}

	void Sprite::SetSpriteSize(const glm::vec2 &spriteSize)
	{
		mSpriteSize = spriteSize;
		SetCoords(mCoordinates, mCellSize, mSpriteSize);
	}

	const glm::vec2 &Sprite::GetMinCoords() const
	{
		return mCoords[0];
	}

	const glm::vec2 &Sprite::GetMaxCoords() const
	{
		return mCoords[2];
	}

	void Sprite::SetSourceTexture(const TAssetHandle<Texture2D> &texture)
	{
		mSource = texture;
	}

	void Sprite::Serialize(StreamWriter& ar) const
	{
		Asset::Serialize(ar);
		ar(mSpriteSize, mCellSize, mCoordinates, mSource);
	}

	void Sprite::Deserialize(StreamReader& ar)
	{
		Asset::Deserialize(ar);
		ar(mSpriteSize, mCellSize, mCoordinates, mSource);
		SetCoords(mCoordinates, mCellSize, mSpriteSize);
	}


	REFLECT(Sprite)
	{
		BEGIN_REFLECT(Sprite)
			REFLECT_CONSTRUCTOR()
			REFLECT_PROPERTY("Source", GetSourceTexture, SetSourceTexture)
			REFLECT_PROPERTY("Coordinates", GetCoordinates, SetCoordinates)
			REFLECT_PROPERTY("CellSize", GetCellSize, SetCellSize)
			REFLECT_PROPERTY("SpriteSize", GetSpriteSize, SetSpriteSize);
	}

}