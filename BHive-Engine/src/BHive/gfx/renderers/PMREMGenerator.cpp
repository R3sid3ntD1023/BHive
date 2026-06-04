#include "gfx/Framebuffer.h"
#include "gfx/RenderCommand.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "PMREMGenerator.h"
#include "gfx/Pipeline.h"
#include "Renderer.h"

namespace BHive
{
	#define PMREM_EQUIRECTANGULAR "Equirectangular.glsl"
	#define PMREM_CONVOLUTION "Irradiance.glsl"
	#define PMREM_PREFILTER "PrefilterEnvironment.glsl"
	#define PMREM_BRDFLUT "BRDFLut.glsl"

	#define PMREM_EQUIRECTANGULAR_PIPELINE "PMREM_Equirectangular"
	#define PMREM_CONVOLUTION_PIPELINE "PMREM_Convolution"
	#define PMREM_PREFILTER_PIPELINE "PMREM_PreFilter"
	#define PMREM_BRDFLUT_PIPELINE "BRDF_LUT"

	PMREMGenerator::PMREMGenerator(const PMREMSettings &settings)
		: mSettings(settings)
	{
		{
			auto EquirectangularShader = ShaderManager::Get(PMREM_EQUIRECTANGULAR);
			Pipeline::ComputePipelineState state{};
			state.ShaderProgram = EquirectangularShader;
			PipelineRegistry::Register(PMREM_EQUIRECTANGULAR_PIPELINE, state);
		}

		{
			auto IrradianceShader = ShaderManager::Get(PMREM_CONVOLUTION);
			Pipeline::ComputePipelineState state{};
			state.ShaderProgram = IrradianceShader;
			PipelineRegistry::Register(PMREM_CONVOLUTION_PIPELINE, state);
		}

		{
			auto PreFilterEnvironmentShader = ShaderManager::Get(PMREM_PREFILTER);
			Pipeline::ComputePipelineState state{};
			state.ShaderProgram = PreFilterEnvironmentShader;
			PipelineRegistry::Register(PMREM_PREFILTER_PIPELINE, state);
		}

		{
			auto BRDFLUTShader = ShaderManager::Get(PMREM_BRDFLUT);
			Pipeline::ComputePipelineState state{};
			state.ShaderProgram = BRDFLUTShader;
			PipelineRegistry::Register(PMREM_BRDFLUT_PIPELINE, state);
		}
	}

	Ref<TextureCube> PMREMGenerator::GenerateEnvironmentCubeMap(const Ref<Texture2D> &tex)
	{
		FTextureCreateInfo create_info{};
		create_info.Format = EFormat::RGBA32F;
		create_info.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		create_info.MinFilter = EMinFilter::LINEAR;
		create_info.ArrayLayers = 6;
		create_info.MipLevels = mSettings.PrefilterMipLevels;
		create_info.DebugName = "EnvironmentCube";
		create_info.Roles |= ETextureRole::ComputeWrite;
		auto cube =  TextureCube::Create(mSettings.EnvironmentMapSize, create_info);

		Renderer::Get().ExecuteComputePass(
					PipelineRegistry::Get(PMREM_EQUIRECTANGULAR_PIPELINE), {(mSettings.EnvironmentMapSize + 7) / 8, (mSettings.EnvironmentMapSize + 7) / 8, 6},
					[=](FComputeBindings &b)
					{
						b.SampledImage("equirectangularMap", FImageInfo{tex});
						b.StorageImage("imgOutput", FImageInfo{cube, 0, 1, 0, 6, EImageAccess::WRITE});
						b.Set("u_width", mSettings.EnvironmentMapSize);
						b.Set("u_height", mSettings.EnvironmentMapSize);
					});

		Renderer::Get().ExecuteTransferPass([=](ITransferContext&) { cube->GenerateMips();});
		
		return cube;
	}

	Ref<TextureCube> PMREMGenerator::GenerateIrradianceMap(const Ref<TextureCube> &tex)
	{
		FTextureCreateInfo create_info{};
		create_info.Format = EFormat::RGBA32F;
		create_info.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		create_info.MinFilter = EMinFilter::LINEAR;
		create_info.Roles |= ETextureRole::ComputeWrite;
		create_info.DebugName = "Irradiance";
		auto irradiance = TextureCube::Create(mSettings.IrradianceSize, create_info);

		Renderer::Get().ExecuteComputePass(
			PipelineRegistry::Get(PMREM_CONVOLUTION_PIPELINE), {(mSettings.EnvironmentMapSize + 7) / 8, (mSettings.EnvironmentMapSize + 7) / 8, 1},
			[=](FComputeBindings &b)
			{
				b.SampledImage("environmentMap", {tex});
				b.StorageImage("irradianceMap", FImageInfo{irradiance, 0, 1, 0, 6, EImageAccess::WRITE});
				b.Set("u_width", mSettings.IrradianceSize);
				b.Set("u_height", mSettings.IrradianceSize);
			});

		return irradiance;
	}

	Ref<TextureCube> PMREMGenerator::GeneratePreFilteredEnvironmentMap(const Ref<TextureCube> &tex)
	{
		FTextureCreateInfo create_info{};
		create_info.Format = EFormat::RGBA16F;
		create_info.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		create_info.MinFilter = EMinFilter::MIPMAP_LINEAR;
		create_info.MagFilter = EMagFilter::LINEAR;
		create_info.MipLevels = mSettings.PrefilterMipLevels;
		create_info.Roles |= ETextureRole::ComputeWrite;
		create_info.DebugName = "PreFilterEnvironment";
		auto prefilter = TextureCube::Create(mSettings.PrefilterMapSize, create_info);

		for (uint32_t mip = 0; mip < mSettings.PrefilterMipLevels; mip++)
		{
			uint32_t s = mSettings.PrefilterMapSize >> mip;
			if (s == 0)
				s = 1;

			float roughness = (float)mip / (float)(mSettings.PrefilterMipLevels - 1);

			Renderer::Get().ExecuteComputePass(
				PipelineRegistry::Get(PMREM_PREFILTER_PIPELINE), {(s + 7) / 8, (s + 7) / 8, 6},
				[=](FComputeBindings &b)
				{
					b.SampledImage("environmentMap", {tex});
					b.StorageImage("imgOutput", FImageInfo{prefilter, mip, 1, 0, 6, EImageAccess::WRITE});
					b.Set("u_envResolution", mSettings.EnvironmentMapSize);
					b.Set("u_roughness", roughness);
					b.Set("u_mip_level", int32_t(mip));
					b.Set("u_width", s);
					b.Set("u_height", s);
				});
		}

		return prefilter;
	}

	Ref<Texture2D> PMREMGenerator::GenerateBRDFLUTMap()
	{

		FTextureCreateInfo brdfLUTCreateInfo{};
		brdfLUTCreateInfo.Format = EFormat::RG16F;
		brdfLUTCreateInfo.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		brdfLUTCreateInfo.MagFilter = EMagFilter::NEAREST;
		brdfLUTCreateInfo.MinFilter = EMinFilter::NEAREST;
		brdfLUTCreateInfo.Roles |= ETextureRole::ComputeWrite;
		brdfLUTCreateInfo.DebugName = "BRDFLUT Texture";

		auto brdfLUT = Texture2D::Create({mSettings.BrdfLutSize, mSettings.BrdfLutSize}, brdfLUTCreateInfo);
		Renderer::Get().ExecuteComputePass(
			PipelineRegistry::Get(PMREM_BRDFLUT_PIPELINE), {mSettings.BrdfLutSize / 8, mSettings.BrdfLutSize / 8, 1}, [=](FComputeBindings &b) { b.StorageImage("brdfLutTexture", FImageInfo{brdfLUT}); });

		return brdfLUT;
	}
} // namespace BHive
