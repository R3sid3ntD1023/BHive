#pragma once

#include "TAsset.h"
#include "sprite/Sprite.h"

namespace BHive
{
	class ATexture2D;

	class ASprite : public TAsset<Sprite>
	{
	private:
		Ref<ATexture2D> mSource;

	public:
		ASprite() = default;
		ASprite(const Ref<ATexture2D> &source, const glm::vec2 &coords, const glm::vec2 &cell, const glm::vec2 &size);
		~ASprite() = default;

		void Save(cereal::BinaryOutputArchive &ar) const;
		void Load(cereal::BinaryInputArchive &ar);

		REFLECTABLEV(TAsset);
	};

	REFLECT_EXTERN(ASprite)
} // namespace BHive
