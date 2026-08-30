#include "IResourceFactory.h"
#include "gfx/material/Material.h"

namespace BHive
{
	struct BHIVE_API MaterialFactory : IResourceFactory<Material>
	{
		static MaterialPtr Create(const std::string &shaderName);

		static MaterialPtr CreateLambert();

		static MaterialPtr CreateStandard();

		static MaterialPtr CreateEmissive();
	};
} // namespace BHive