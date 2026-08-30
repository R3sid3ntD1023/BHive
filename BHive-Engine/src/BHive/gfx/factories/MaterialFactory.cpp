#include "MaterialFactory.h"
#include "gfx/material/LambertMaterial.h"
#include "gfx/material/EmissiveMaterial.h"
#include "gfx/material/StandardMaterial.h"

namespace BHive
{
	ResourceHandle MaterialFactory::Create(const std::string &shaderName)
	{
		return CreateResource<Material>(shaderName);
	}

	ResourceHandle MaterialFactory::CreateLambert()
	{
		return CreateResource<LambertMaterial>();
	}

	ResourceHandle MaterialFactory::CreateStandard()
	{
		return CreateResource<StandardMaterial>();
	}

	ResourceHandle MaterialFactory::CreateEmissive()
	{
		return CreateResource<EmissiveMaterial>();
	}

} // namespace BHive
