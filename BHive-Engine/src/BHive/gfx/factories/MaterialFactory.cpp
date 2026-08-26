#include "MaterialFactory.h"
#include "gfx/material/LambertMaterial.h"
#include "gfx/material/EmissiveMaterial.h"
#include "gfx/material/StandardMaterial.h"

namespace BHive
{
	ResourceHandle MaterialFactory::Create(const std::string &shaderName)
	{
		return GetResourceRegsitry()->CreateResource<Material>(shaderName);
	}

	ResourceHandle MaterialFactory::CreateLambert()
	{
		return GetResourceRegsitry()->CreateResource<LambertMaterial>();
	}

	ResourceHandle MaterialFactory::CreateStandard()
	{
		return GetResourceRegsitry()->CreateResource<StandardMaterial>();
	}

	ResourceHandle MaterialFactory::CreateEmissive()
	{
		return GetResourceRegsitry()->CreateResource<EmissiveMaterial>();
	}

} // namespace BHive
