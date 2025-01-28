#include "physics/PhysicsMaterial.h"
#include "PhysicsMaterialFactory.h"

namespace BHive
{
	Ref<Asset> PhysicsMaterialFactory::CreateNew()
	{
		auto pm = CreateRef<PhysicsMaterial>();
		OnAssetCreated.invoke(pm);
		return pm;
	}

	REFLECT_FACTORY(PhysicsMaterialFactory)
} // namespace BHive