#include "PhysicsMaterialFactory.h"
#include "physics/PhysicsMaterial.h"

namespace BHive
{
	Ref<Asset> PhysicsMaterialFactory::CreateNew()
	{
		auto pm =  CreateRef<PhysicsMaterial>();
		OnImportCompleted.invoke(pm);
		return pm;
	}

    REFLECT_Factory(PhysicsMaterialFactory, PhysicsMaterial, ".physicsmaterial") 
} // namespace BHive