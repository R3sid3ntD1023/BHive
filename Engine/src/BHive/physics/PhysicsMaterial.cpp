#include "PhysicsMaterial.h"

namespace BHive
{
	void PhysicsMaterial::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);
		ar(mFrictionCoefficient, mBounciness, mMassDensity);
	}

	void PhysicsMaterial::Load(cereal::BinaryInputArchive &ar)
	{
		Asset::Load(ar);
		ar(mFrictionCoefficient, mBounciness, mMassDensity);
	}

} // namespace BHive
