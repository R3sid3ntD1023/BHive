#include "EmissiveMaterial.h"

namespace BHive
{
	EmissiveMaterial::EmissiveMaterial()
		: Material("EmissiveMaterial.glsl")
	{

		SetParam("Emission", MaterialParam(mEmissionColor));
		SetTexture("EmissionMap", {});
	}

	IMaterial &EmissiveMaterial::SetEmissionColor(FColor color)
	{
		mEmissionColor = color;
		return SetParam("Emission", MaterialParam(mEmissionColor));
	}

	void EmissiveMaterial::Save(cereal::BinaryOutputArchive &ar) const
	{
		Material::Save(ar);
		ar(MAKE_NVP("Emission", mEmissionColor));
	}

	void EmissiveMaterial ::Load(cereal::BinaryInputArchive &ar)
	{
		Material::Load(ar);
		ar(MAKE_NVP("Emission", mEmissionColor));
	}

	REFLECT(EmissiveMaterial)
	{
		BEGIN_REFLECT(EmissiveMaterial)
		REFLECT_PROPERTY(mEmissionColor)(META_DATA(EPropertyMetaData_HDR, true)) REFLECT_CONSTRUCTOR();
	}
} // namespace BHive