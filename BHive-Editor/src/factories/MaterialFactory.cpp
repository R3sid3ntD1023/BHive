#include "material/Material.h"
#include "MaterialFactory.h"
#include "material/StandardMaterial.h"
#include "material/LambertMaterial.h"
#include "material/EmissiveMaterial.h"

namespace BHive
{
	Ref<Asset> StandardMaterialFactory::CreateNew()
	{
		auto material = CreateRef<StandardMaterial>();
		OnAssetCreated.invoke(material);
		return material;
	}

	Ref<Asset> LambertMaterialFactory::CreateNew()
	{
		auto material = CreateRef<LambertMaterial>();
		OnAssetCreated.invoke(material);
		return material;
	}

	Ref<Asset> EmissiveMaterialFactory::CreateNew()
	{
		auto material = CreateRef<EmissiveMaterial>();
		OnAssetCreated.invoke(material);
		return material;
	}

	REFLECT_FACTORY(StandardMaterialFactory)
	REFLECT_FACTORY(LambertMaterialFactory)
	REFLECT_FACTORY(EmissiveMaterialFactory)
} // namespace BHive