#include "PhysicsMaterial.h"

namespace BHive
{
	void PhysicsMaterial::Serialize(StreamWriter& ar) const
	{
		ar(mFrictionCoefficient, mBounciness, mMassDensity);
	}

	void PhysicsMaterial::Deserialize(StreamReader& ar)
	{
		ar(mFrictionCoefficient, mBounciness, mMassDensity);
	}

	REFLECT(PhysicsMaterial)
	{
		BEGIN_REFLECT(PhysicsMaterial)
			REFLECT_PROPERTY("FrictionCoefficient", mFrictionCoefficient)
			REFLECT_PROPERTY("Bounciness", mBounciness)
			REFLECT_PROPERTY("MassDensity", mMassDensity);
	}
}
