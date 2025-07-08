#include "material/Material.h"
#include "MaterialFactory.h"
#include "material/BDRFMaterial.h"

namespace BHive
{
	Ref<Asset> MaterialFactory::CreateNew()
	{
		auto material = CreateRef<BDRFMaterial>();
		OnAssetCreated.invoke(material);
		return material;
	}

	REFLECT_FACTORY(MaterialFactory)
} // namespace BHive