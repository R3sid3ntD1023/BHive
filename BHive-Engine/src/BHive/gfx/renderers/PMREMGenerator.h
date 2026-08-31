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
		PMREMGenerator() = default;

		void Initialize(const PMREMSettings &settings = {});

		PMREMResult GenerateEnvironmentMaps(Texture2DPtr hdr);

		auto &GetEnvironmentCube() const { return mEnvironmentTextures.Environment; }
		auto &GetIrradiance() const { return mEnvironmentTextures.Irradiance; }
		auto &GetPreFilter() const { return mEnvironmentTextures.PreFilter; }

	private:
		void InitializeTextures();

		void DoEquirectangularConversion(IRendererContext &ctx);
		void DoGenerateCubeMips(IRendererContext &ctx);
		void DoConvolution(IRendererContext &ctx);
		void DoPreFilter(IRendererContext &ctx, uint32_t mip);

	private:
		PMREMSettings mSettings{};
		PMREMResult mEnvironmentTextures;
		Texture2DPtr mInput;
	};

	class BHIVE_API BRDFLUTGenerator
	{
	public:
		static Texture2DPtr GenerateBRDFLUTMap(uint32_t size = 512);
	};
} // namespace BHive