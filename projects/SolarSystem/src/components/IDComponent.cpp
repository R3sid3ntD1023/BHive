#include "IDComponent.h"
#include "CelestrialBody.h"

BEGIN_NAMESPACE(BHive)

void IDComponent::Save(cereal::JSONOutputArchive &ar) const
{
	ar(MAKE_NVP("ID", mID));
};

void IDComponent::Load(cereal::JSONInputArchive &ar)
{
	ar(MAKE_NVP("ID", mID));
};

REFLECT(IDComponent)
{
	BEGIN_REFLECT(IDComponent)
	REFLECT_CONSTRUCTOR();
}

END_NAMESPACE