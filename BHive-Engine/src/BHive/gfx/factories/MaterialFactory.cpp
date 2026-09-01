#include "MaterialFactory.h"
#include "gfx/material/EmissiveMaterial.h"
#include "gfx/material/LambertMaterial.h"
#include "gfx/material/StandardMaterial.h"

namespace BHive
{
	MaterialPtr MaterialFactory::Create(const std::string &shaderName)
	{
		return CreateResource<Material>(shaderName);
	}

	MaterialPtr MaterialFactory::CreateLambert()
	{
		return CreateResource<LambertMaterial>();
	}

	MaterialPtr MaterialFactory::CreateStandard()
	{
		return CreateResource<StandardMaterial>();
	}

	MaterialPtr MaterialFactory::CreateEmissive()
	{
		return CreateResource<EmissiveMaterial>();
	}

} // namespace BHive
