#pragma once

#include "core/Core.h"
#include "gfx/registries/Handles.h"

namespace BHive
{
	struct IRendererContext;

	struct PMREMSettings
	{
		uint32_t EnvironmentMapSize = 512;
		uint32_t PrefilterMapSize = 128;
		uint32_t PrefilterMipLevels = 5;
		uint32_t IrradianceSize = 32;
	};

	struct PMREMResult
	{
		TextureCubePtr Environment;
		TextureCubePtr Irradiance;
		TextureCubePtr PreFilter;

		bool IsValid() const { return Environment && Irradiance && PreFilter; }
	};

	class BHIVE_API PMREMGenerator
	{
	public:
		static void Initialize(const PMREMSettings &settings = {});

		static PMREMResult GenerateEnvironmentMaps(Texture2DPtr hdr);

	private:
		static PMREMResult InitializeTextures();

	private:
		static inline PMREMSettings mSettings{};
	};

	class BHIVE_API BRDFLUTGenerator
	{
	public:
		static Texture2DPtr GenerateBRDFLUTMap(uint32_t size = 512);
	};
} // namespace BHive