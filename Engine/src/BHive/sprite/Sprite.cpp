#include "gfx/textures/Texture2D.h"
#include "Sprite.h"

namespace BHive
{
	Sprite::Sprite(const Sprite &other)
		: mSource(other.mSource),
		  mCoords(other.mCoords),
		  mMin(other.mMin),
		  mMax(other.mMax)
	{
	}

	Sprite::Sprite(
		const Ref<Texture2D> &texture, const glm::vec2 &coords, const glm::vec2 &cellSize, const glm::vec2 spriteSize)
		: mSource(texture)

	{
		CalculateMinMax(coords, cellSize, spriteSize);
		Initialize();
	}

	Sprite::Sprite(const Ref<Texture2D> &texture, const glm::vec2 &min, const glm::vec2 &max)
		: mSource(texture),
		  mMin(min),
		  mMax(max)

	{
		Initialize();
	}

	void Sprite::CalculateMinMax(const glm::vec2 &coords, const glm::vec2 &cellSize, const glm::vec2 spriteSize)
	{
		auto texture = mSource;
		glm::vec2 texture_size = {texture->GetWidth(), texture->GetHeight()};
		mMin = (coords * cellSize) / texture_size;
		mMax = ((coords + spriteSize) * cellSize) / texture_size;
	}

	void Sprite::Initialize()
	{
		mCoords[0] = mMin;
		mCoords[1] = {mMax.x, mMin.y};
		mCoords[2] = mMax;
		mCoords[3] = {mMin.x, mMax.y};
	}

	void Sprite::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);
		ar(mMin, mMax, mSource);
	}

	void Sprite::Load(cereal::BinaryInputArchive &ar)
	{
		Asset::Load(ar);
		ar(mMin, mMax, mSource);

		Initialize();
	}

	REFLECT(Sprite)
	{
		BEGIN_REFLECT(Sprite)
		REFLECT_CONSTRUCTOR();
	}

} // namespace BHive