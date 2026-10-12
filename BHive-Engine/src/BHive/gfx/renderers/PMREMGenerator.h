#pragma once

#include "core/Core.h"

namespace BHive
{
	struct IRendererContext;
	class Texture2D;
	class TextureCube;

	struct PMREMSettings
	{
		uint32_t EnvironmentMapSize = 512;
		uint32_t PrefilterMapSize = 128;
		uint32_t PrefilterMipLevels = 8;
		uint32_t IrradianceSize = 32;
	};

	struct PMREMResult
	{
		Ref<TextureCube> Environment;
		Ref<TextureCube> Irradiance;
		Ref<TextureCube> PreFilter;

		bool IsValid() const { return Environment && Irradiance && PreFilter; }
	};

	class BHIVE_API PMREMGenerator
	{
	public:
		static void Initialize(const PMREMSettings &settings = {});

		static PMREMResult GenerateEnvironmentMaps(Ref<Texture2D> hdr);

	private:
		static PMREMResult InitializeTextures();

	private:
		static inline PMREMSettings mSettings{};
	};

	class BHIVE_API BRDFLUTGenerator
	{
	public:
		static Ref<Texture2D> GenerateBRDFLUTMap(uint32_t size = 512);
	};
} // namespace BHive