#pragma once

#include "core/Core.h"

namespace BHive
{
	class TextureCube;
	class Texture2D;
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
		Ref<TextureCube> Environment;
		Ref<TextureCube> Irradiance;
		Ref<TextureCube> PreFilter;

		bool IsValid() const { return Environment && Irradiance && PreFilter; }
	};

	class BHIVE_API PMREMGenerator
	{
	public:
		PMREMGenerator() = default;

		void Initialize(const PMREMSettings &settings = {});

		PMREMResult GenerateEnvironmentMaps(const Ref<Texture2D> &hdr);

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
		Ref<Texture2D> mInput;
	};

	class BHIVE_API BRDFLUTGenerator
	{
	public:
		static Ref<Texture2D> GenerateBRDFLUTMap(uint32_t size = 512);
	};
} // namespace BHive