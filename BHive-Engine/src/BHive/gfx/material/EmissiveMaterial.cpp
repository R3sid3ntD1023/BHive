#include "EmissiveMaterial.h"

namespace BHive
{
	void EmissiveMaterial::Save(cereal::BinaryOutputArchive &ar) const
	{
		Material::Save(ar);
		ar(MAKE_NVP("Emission", EmissionColor));
	}

	void EmissiveMaterial ::Load(cereal::BinaryInputArchive &ar)
	{
		Material::Load(ar);
		ar(MAKE_NVP("Emission", EmissionColor));
	}

	void EmissiveMaterial ::Submit(Pipeline* pipeline)
	{
		mBackendMaterial->Set("EmissiveColor", EmissionColor);

		Material::Submit(pipeline);
	}

	REFLECT(EmissiveMaterial)
	{
		BEGIN_REFLECT(EmissiveMaterial)
		REFLECT_PROPERTY(EmissionColor)(META_DATA(EPropertyMetaData_HDR, true)) REFLECT_CONSTRUCTOR();
	}
} // namespace BHive