#include "ASprite.h"
#include "ATexture2D.h"
#include "asset/AssetManager.h"

namespace BHive
{
	ASprite::ASprite(const Ref<ATexture2D> &source, const glm::vec2 &coords, const glm::vec2 &cell, const glm::vec2 &size)
		: TAsset(Sprite::Create(source->Get(), coords, cell, size)),
		  mSource(source)
	{
	}

	void ASprite::Save(cereal::BinaryOutputArchive &ar) const
	{
		TAsset::Save(ar);
		ar(mSource, mObject->GetCoords()[0], mObject->GetCoords()[2]);
	}

	void ASprite::Load(cereal::BinaryInputArchive &ar)
	{
		TAsset::Load(ar);
		glm::vec2 min, max;
		ar(mSource, min, max);

		mObject = Sprite::Create(mSource->Get(), min, max);
	}

	REFLECT(ASprite)
	{
		BEGIN_REFLECT(ASprite, "Sprite").constructor();
	}
} // namespace BHive