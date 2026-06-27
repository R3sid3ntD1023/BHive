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

	void PMREMGenerator::Initialize(const PMREMSettings &settings)
	{
		mSettings = settings;
		InitializePipelines();
		InitializeTextures();
	}

	PMREMResult PMREMGenerator::GenerateEnvironmentMaps(const Ref<Texture2D> & hdr)
	{
		mInput = hdr;

		auto &pass = RenderCommand::BeginPass("Generate PMREM Maps", EPassType::OffScreen);

		//Phase 0 : equirectangular -> cubemap
		pass.BeginPhase("Convert 2D -> cube");
		pass.Push(mInput, EImageAccess::ComputeSampled);
		pass.Push(mEnvironmentTextures.Environment, EImageAccess::ComputeStorageWrite, {0, 1, 0, 6});
		pass.Push("Compute CubeMap", this, &PMREMGenerator::DoEquirectangularConversion);
		pass.EndPhase();

		// Phase 1 : generate mipmaps for environment
		pass.BeginPhase("Gen Mips for Environment Cube");
		pass.Push("Generate MipMaps", this, &PMREMGenerator::DoGenerateCubeMips);
		pass.EndPhase();

		//Phase 2 : Irradiance convolution
		pass.BeginPhase("Convolution");
		pass.Push(mEnvironmentTextures.Environment, EImageAccess::ComputeSampled);
		pass.Push(mEnvironmentTextures.Irradiance, EImageAccess::ComputeStorageWrite, {0, 1, 0, 6});
		pass.Push("Compute Irradiance", this, &PMREMGenerator::DoConvolution);
		pass.EndPhase();

		//Phase 3 - N: Prefilter Specular Mip Chain

		for (uint32_t mip = 0; mip < mSettings.PrefilterMipLevels; mip++)
		{
			pass.BeginPhase(std::format("Prefiltering Mip {}", mip ));
			pass.Push(mEnvironmentTextures.Environment, EImageAccess::ComputeSampled);
			pass.Push(mEnvironmentTextures.PreFilter, EImageAccess::ComputeStorageWrite, {mip, 1, 0, 6});
			pass.Push("Compute PreFilter", this, &PMREMGenerator::DoPreFilter, mip);
			pass.EndPhase();
		}

		RenderCommand::EndPass();

		return mEnvironmentTextures;
	}

	void PMREMGenerator::InitializePipelines()
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
	}

	void PMREMGenerator::InitializeTextures()
	{
		

		FTextureCreateInfo cubeInfo{};
		cubeInfo.Format = EFormat::RGBA32F;
		cubeInfo.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		cubeInfo.MinFilter = EMinFilter::LINEAR;
		cubeInfo.ArrayLayers = 6;
		cubeInfo.MipLevels = mSettings.PrefilterMipLevels;
		cubeInfo.DebugName = "EnvironmentCube";
		cubeInfo.Roles |= ETextureRole::ComputeWrite;
		mEnvironmentTextures.Environment = TextureCube::Create(mSettings.EnvironmentMapSize, cubeInfo);

		FTextureCreateInfo convolutionInfo{};
		convolutionInfo.Format = EFormat::RGBA32F;
		convolutionInfo.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		convolutionInfo.MinFilter = EMinFilter::LINEAR;
		convolutionInfo.Roles |= ETextureRole::ComputeWrite;
		convolutionInfo.DebugName = "Irradiance";
		mEnvironmentTextures.Irradiance = TextureCube::Create(mSettings.IrradianceSize, convolutionInfo);

		FTextureCreateInfo preFilteredInfo{};
		preFilteredInfo.Format = EFormat::RGBA16F;
		preFilteredInfo.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		preFilteredInfo.MinFilter = EMinFilter::MIPMAP_LINEAR;
		preFilteredInfo.MagFilter = EMagFilter::LINEAR;
		preFilteredInfo.MipLevels = mSettings.PrefilterMipLevels;
		preFilteredInfo.Roles |= ETextureRole::ComputeWrite;
		preFilteredInfo.DebugName = "PreFilterEnvironment";
		mEnvironmentTextures.PreFilter = TextureCube::Create(mSettings.PrefilterMapSize, preFilteredInfo);

	}

	void PMREMGenerator::DoEquirectangularConversion(IRendererContext &ctx)
	{
		Renderer::Get().ExecuteComputePass(
			PipelineRegistry::Get(PMREM_EQUIRECTANGULAR_PIPELINE), {(mSettings.EnvironmentMapSize + 7) / 8, (mSettings.EnvironmentMapSize + 7) / 8, 6},
			[=](FComputeBindings &b)
			{
				b.Bind("equirectangularMap", mInput);
				b.Bind("imgOutput", mEnvironmentTextures.Environment, {0, 1, 0, 6});
				b.Set("u_width", mSettings.EnvironmentMapSize);
				b.Set("u_height", mSettings.EnvironmentMapSize);
			});
	}

	void PMREMGenerator::DoGenerateCubeMips(IRendererContext &ctx)
	{
		Renderer::Get().ExecuteTransferPass([=](ITransferContext &) { mEnvironmentTextures.Environment->GenerateMips(); });
	}

	void PMREMGenerator::DoConvolution(IRendererContext &ctx)
	{
		Renderer::Get().ExecuteComputePass(
			PipelineRegistry::Get(PMREM_CONVOLUTION_PIPELINE), {(mSettings.EnvironmentMapSize + 7) / 8, (mSettings.EnvironmentMapSize + 7) / 8, 1},
			[=](FComputeBindings &b)
			{
				b.Bind("environmentMap", mEnvironmentTextures.Environment);
				b.Bind("irradianceMap", mEnvironmentTextures.Irradiance, {0, 1, 0, 6});
				b.Set("u_width", mSettings.IrradianceSize);
				b.Set("u_height", mSettings.IrradianceSize);
			});
	}

	void PMREMGenerator::DoPreFilter(IRendererContext &ctx, uint32_t mip)
	{
		uint32_t s = mSettings.PrefilterMapSize >> mip;
		if (s == 0)
			s = 1;

		float roughness = (float)mip / (float)(mSettings.PrefilterMipLevels - 1);

		Renderer::Get().ExecuteComputePass(
			PipelineRegistry::Get(PMREM_PREFILTER_PIPELINE), {(s + 7) / 8, (s + 7) / 8, 6},
			[=](FComputeBindings &b)
			{
				b.Bind("environmentMap", mEnvironmentTextures.Environment);
				b.Bind("imgOutput", mEnvironmentTextures.PreFilter, {mip, 1, 0, 6});
				b.Set("u_envResolution", mSettings.EnvironmentMapSize);
				b.Set("u_roughness", roughness);
				b.Set("u_mip_level", int32_t(mip));
				b.Set("u_width", s);
				b.Set("u_height", s);
			});
	}

	Ref<Texture2D> BRDFLUTGenerator::GenerateBRDFLUTMap(uint32_t size)
	{
		auto BRDFLUTShader = ShaderManager::Get(PMREM_BRDFLUT);
		Pipeline::ComputePipelineState state{};
		state.ShaderProgram = BRDFLUTShader;
		PipelineRegistry::Register(PMREM_BRDFLUT_PIPELINE, state);

		FTextureCreateInfo brdfLUTInfo{};
		brdfLUTInfo.Format = EFormat::RG16F;
		brdfLUTInfo.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		brdfLUTInfo.MagFilter = EMagFilter::NEAREST;
		brdfLUTInfo.MinFilter = EMinFilter::NEAREST;
		brdfLUTInfo.Roles |= ETextureRole::ComputeWrite;
		brdfLUTInfo.DebugName = "BRDFLUT Texture";

		auto brdfLUT = Texture2D::Create({size, size}, brdfLUTInfo);

		Renderer::Get().ExecuteComputePass(
			PipelineRegistry::Get(PMREM_BRDFLUT_PIPELINE), {size / 8, size / 8, 1}, [&](FComputeBindings &b) { b.Bind("brdfLutTexture", brdfLUT); });

		return brdfLUT;		

	}

} // namespace BHive
