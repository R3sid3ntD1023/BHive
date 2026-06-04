#pragma once

#include "core/Core.h"

namespace BHive
{
	class TextureCube;
	class Texture2D;

	struct PMREMSettings
	{
		uint32_t EnvironmentMapSize = 512;
		uint32_t PrefilterMapSize = 128;
		uint32_t PrefilterMipLevels = 5;
		uint32_t IrradianceSize = 32;
		uint32_t BrdfLutSize = 512;
	};

	class BHIVE_API PMREMGenerator
	{
	public:
		PMREMGenerator(const PMREMSettings &settings = {});

		Ref<TextureCube> GenerateEnvironmentCubeMap(const Ref<Texture2D>& tex);

		Ref<TextureCube> GenerateIrradianceMap(const Ref<TextureCube>& tex);

		Ref<TextureCube> GeneratePreFilteredEnvironmentMap(const Ref<TextureCube> &tex);

		Ref<Texture2D> GenerateBRDFLUTMap();

	private:
		PMREMSettings mSettings{};
	};
} // namespace BHive