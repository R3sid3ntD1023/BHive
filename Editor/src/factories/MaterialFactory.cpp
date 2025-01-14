#include "MaterialFactory.h"
#include "material/Material.h"
#include "asset/AssetSerializer.h"

namespace BHive
{
	Ref<Asset> MaterialFactory::CreateNew()
	{
		auto material = CreateRef<Material>();
		OnImportCompleted.invoke(material);
		return material;
	}


    REFLECT_Factory(MaterialFactory, Material, ".material")
} // namespace BHive