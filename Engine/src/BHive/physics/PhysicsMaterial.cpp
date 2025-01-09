#include "PhysicsMaterial.h"

namespace BHive
{
	void PhysicsMaterial::Serialize(StreamWriter& ar) const
	{
		Asset::Serialize(ar);
		ar(mFrictionCoefficient, mBounciness, mMassDensity);
	}

	void PhysicsMaterial::Deserialize(StreamReader& ar)
	{
		Asset::Deserialize(ar);
		ar(mFrictionCoefficient, mBounciness, mMassDensity);
	}

	REFLECT(PhysicsMaterial)
	{
		BEGIN_REFLECT(PhysicsMaterial)
		REFLECT_CONSTRUCTOR()
			REFLECT_PROPERTY("FrictionCoefficient", mFrictionCoefficient)
			REFLECT_PROPERTY("Bounciness", mBounciness)
			REFLECT_PROPERTY("MassDensity", mMassDensity);
	}
}
