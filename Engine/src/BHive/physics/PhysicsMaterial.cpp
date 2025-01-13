#include "PhysicsMaterial.h"

namespace BHive
{
	void PhysicsMaterial::Save(cereal::JSONOutputArchive& ar) const
	{
		Asset::Save(ar);
		ar(MAKE_NVP("FrictionCoefficient", mFrictionCoefficient),
		   MAKE_NVP("Bounciness", mBounciness), MAKE_NVP("MassDensity", mMassDensity));
	}

	void PhysicsMaterial::Load(cereal::JSONInputArchive &ar)
	{
		Asset::Load(ar);
		ar(MAKE_NVP("FrictionCoefficient", mFrictionCoefficient),
		   MAKE_NVP("Bounciness", mBounciness), MAKE_NVP("MassDensity", mMassDensity));
	}

	REFLECT(PhysicsMaterial)
	{
		BEGIN_REFLECT(PhysicsMaterial)
		REFLECT_CONSTRUCTOR()
		REFLECT_PROPERTY("FrictionCoefficient", mFrictionCoefficient)
		REFLECT_PROPERTY("Bounciness", mBounciness)
		REFLECT_PROPERTY("MassDensity", mMassDensity);
	}
} // namespace BHive
