#include "material/Material.h"
#include "MaterialFactory.h"

namespace BHive
{
	Ref<Asset> MaterialFactory::CreateNew()
	{
		auto material = CreateRef<Material>();
		OnAssetCreated.invoke(material);
		return material;
	}

	REFLECT_FACTORY(MaterialFactory, Material, ".material")
} // namespace BHive