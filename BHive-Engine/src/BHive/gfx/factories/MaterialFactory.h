#pragma once

#include "IResourceFactory.h"
#include "gfx/material/Material.h"

namespace BHive
{
	struct BHIVE_API MaterialFactory : public IResourceFactory<IMaterial>
	{
		static MaterialPtr Create(const std::string &shaderName);

		static MaterialPtr CreateLambert();

		static MaterialPtr CreateStandard();

		static MaterialPtr CreateEmissive();

		static MaterialPtr CreateInstance(MaterialPtr parent);
	};
} // namespace BHive