#pragma once

#include "gfx/material/Materials.h"

namespace BHive
{
	struct BHIVE_API MaterialFactory
	{
		static Ref<Material> Create(const std::string &shaderName);

		static Ref<LambertMaterial> CreateLambert();

		static Ref<StandardMaterial> CreateStandard();

		static Ref<EmissiveMaterial> CreateEmissive();

		static Ref<MaterialInstance> CreateInstance(Ref<Material> parent);
	};
} // namespace BHive