#include "PhysicsMaterial.h"

namespace BHive
{
	void PhysicsMaterial::Save(cereal::BinaryOutputArchive &ar) const
	{
		Asset::Save(ar);
		ar(FrictionCoefficient, Bounciness, MassDensity);
	}

	void PhysicsMaterial::Load(cereal::BinaryInputArchive &ar)
	{
		Asset::Load(ar);
		ar(FrictionCoefficient, Bounciness, MassDensity);
	}

	REFLECT(PhysicsMaterial)
	{
		BEGIN_REFLECT(PhysicsMaterial)
		REFLECT_CONSTRUCTOR()
		REFLECT_PROPERTY(FrictionCoefficient) REFLECT_PROPERTY(Bounciness) REFLECT_PROPERTY(MassDensity);
		rttr::type::register_wrapper_converter_for_base_classes<Ref<PhysicsMaterial>>();
	}

} // namespace BHive
