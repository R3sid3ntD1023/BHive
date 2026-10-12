#include "MaterialFactory.h"

namespace BHive
{
	Ref<Material> MaterialFactory::Create(const std::string &shaderName)
	{
		return CreateRef<Material>(shaderName);
	}

	Ref<LambertMaterial> MaterialFactory::CreateLambert()
	{
		return CreateRef<LambertMaterial>();
	}

	Ref<StandardMaterial> MaterialFactory::CreateStandard()
	{
		return CreateRef<StandardMaterial>();
	}

	Ref<EmissiveMaterial> MaterialFactory::CreateEmissive()
	{
		return CreateRef<EmissiveMaterial>();
	}

	Ref<MaterialInstance> MaterialFactory::CreateInstance(Ref<Material> parent)
	{
		return CreateRef<MaterialInstance>(parent);
	}

} // namespace BHive
