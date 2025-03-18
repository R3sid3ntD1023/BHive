#pragma once

#include "TAsset.h"
#include "gfx/textures/Texture2D.h"

namespace BHive
{
	class ATexture2D : public TAsset<Texture2D>
	{
	public:
		ATexture2D() = default;
		ATexture2D(const Ref<Texture2D> &texture);
		~ATexture2D() = default;

		void Save(cereal::BinaryOutputArchive &ar) const;
		void Load(cereal::BinaryInputArchive &ar);

		REFLECTABLEV(TAsset)
	};

	template <typename A>
	void Serialize(A &ar, FTextureSpecification &spec)
	{
		ar(MAKE_NVP(spec.Format), MAKE_NVP(spec.InternalFormat), MAKE_NVP(spec.Channels), MAKE_NVP(spec.WrapMode),
		   MAKE_NVP(spec.MinFilter), MAKE_NVP(spec.MagFilter), MAKE_NVP(spec.BorderColor), MAKE_NVP(spec.Levels),
		   MAKE_NVP(spec.CompareMode), MAKE_NVP(spec.CompareFunc));
	}

	REFLECT_EXTERN(ATexture2D)

} // namespace BHive