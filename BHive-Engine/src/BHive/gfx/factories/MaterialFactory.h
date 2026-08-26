#include "IResourceFactory.h"
#include "gfx/material/Material.h"

namespace BHive
{
	struct BHIVE_API MaterialFactory : IResourceFactory<Material>
	{
		static ResourceHandle Create(const std::string &shaderName);

		static ResourceHandle CreateLambert();

		static ResourceHandle CreateStandard();

		static ResourceHandle CreateEmissive();
	};
} // namespace BHive