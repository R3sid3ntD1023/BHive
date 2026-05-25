#pragma once

#include "core/Core.h"

namespace BHive
{
	class Texture;
	class TextureCube;
	class Texture2D;
	class Pipeline;

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
		

	public:
		PMREMGenerator(const PMREMSettings &settings = {});

		void Initialize();

		void SetEnvironmentMap(const Ref<Texture> &texture);

		const Ref<Texture> &GetIrradianceTexture() const;
		const Ref<Texture> &GetPreFilteredEnvironmentTexture() const;
		const Ref<Texture> &GetEnvironmentCubeTexture() const;
		const Ref<Texture> &GetBDRFLUT() const;

	private:
		void CreateEnvironmentCubeMap();
		void CreateIrradianceMap();
		void CreatePreFilteredEnvironmentMap();
		void CreateBRDFLUTMap();

		Ref<Texture> mEnvironment;
		Ref<TextureCube> mEnvironmentCube;
		Ref<TextureCube> mPreFilteredEnvironment;
		Ref<TextureCube> mIrradiance;
		Ref<Texture2D> mBRDFLUT;

		Ref<Pipeline> mEquirectangularPipeline;
		Ref<Pipeline> mIrradiancePipeline;
		Ref<Pipeline> mBRDFLUTPipeline;
		Ref<Pipeline> mPreFilterEnironmentPipeline;

		PMREMSettings mSettings{};
	};
} // namespace BHive