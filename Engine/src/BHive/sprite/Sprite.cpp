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

	Sprite::Sprite(const Ref<Texture2D> &texture, const FSpriteGenerator &generator)
		: mSource(texture),
		  mGenerator(generator)

	{
		CalculateMinMax(generator);
		Initialize();
	}

	Sprite::Sprite(const Ref<Texture2D> &texture, const glm::vec2 &min, const glm::vec2 &max)
		: mSource(texture),
		  mMin(min),
		  mMax(max)

	{
		Initialize();
	}

	void Sprite::SetCoords(const glm::vec2 &min, const glm::vec2 &max)
	{
		mMin = min;
		mMax = max;
		Initialize();
	}

	void Sprite::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);
		TAssetHandle<Texture2D> handle(mSource);
		ar(handle, mMin, mMax, mGenerator);
	}

	void Sprite::Load(cereal::BinaryInputArchive &ar)
	{
		Asset::Load(ar);
		TAssetHandle<Texture2D> handle(mSource);
		ar(handle, mMin, mMax, mGenerator);

		Initialize();
	}

	void Sprite::SetFromGenerator(const FSpriteGenerator &generator)
	{
		mGenerator = generator;

		if (!mSource)
			return;

		CalculateMinMax(generator);
		Initialize();
	}

	void Sprite::CalculateMinMax(const FSpriteGenerator &generator)
	{
		ASSERT(mSource, "Source texture is null!");

		auto texture = mSource;
		glm::vec2 texture_size = {texture->GetWidth(), texture->GetHeight()};
		mMin = (generator.Coordinates * generator.CellSize) / texture_size;
		mMax = ((generator.Coordinates + generator.Size) * generator.CellSize) / texture_size;
	}

	void Sprite::Initialize()
	{
		mCoords[0] = mMin;
		mCoords[1] = {mMax.x, mMin.y};
		mCoords[2] = mMax;
		mCoords[3] = {mMin.x, mMax.y};
	}

	Ref<Sprite> Sprite::Create(const Ref<Texture2D> &texture, const FSpriteGenerator &generator)
	{
		return CreateRef<Sprite>(texture, generator);
	}

	Ref<Sprite> Sprite::Create(const Ref<Texture2D> &texture, const glm::vec2 &min, const glm::vec2 &max)
	{
		return CreateRef<Sprite>(texture, min, max);
	}

} // namespace BHive