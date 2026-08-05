#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "LambertMaterial.h"
#include "gfx/Pipeline.h"

namespace BHive
{
	LambertMaterial &LambertMaterial::SetDiffuseColor(FColor color)
	{
		mDiffuseColor = color;
		SetParam("DiffuseColor", MaterialParam(color));
		return *this;
	}

	LambertMaterial &LambertMaterial::SetEmissionColor(FColor color)
	{
		mEmissionColor = color;
		SetParam("Emission", MaterialParam(color));
		return *this;
	}

	void LambertMaterial::Save(cereal::BinaryOutputArchive &ar) const
	{
		Material::Save(ar);
		ar(MAKE_NVP("DiffuseColor", mDiffuseColor), MAKE_NVP("EmissionColor", mEmissionColor));
	}

	void LambertMaterial::Load(cereal::BinaryInputArchive &ar)
	{
		Material::Load(ar);
		ar(MAKE_NVP("Color", mDiffuseColor), MAKE_NVP("Emission", mEmissionColor));
	}

	REFLECT(LambertMaterial)
	{
		BEGIN_REFLECT(LambertMaterial)
		REFLECT_PROPERTY(mDiffuseColor)
		REFLECT_PROPERTY(mEmissionColor)
		REFLECT_CONSTRUCTOR();
	}
} // namespace BHive