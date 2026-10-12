#include "PMREMGenerator.h"
#include "Renderer.h"
#include "gfx/Framebuffer.h"
#include "gfx/Pipeline.h"
#include "gfx/RenderCommand.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "gfx/factories/MaterialFactory.h"
#include "gfx/factories/TextureFactory.h"

namespace BHive
{
#define PMREM_EQUIRECTANGULAR "Equirectangular.glsl"
#define PMREM_CONVOLUTION "Irradiance.glsl"
#define PMREM_PREFILTER "PrefilterEnvironment.glsl"
#define PMREM_BRDFLUT "BRDFLut.glsl"

	void PMREMGenerator::Initialize(const PMREMSettings &settings)
	{
		mSettings = settings;
	}

	PMREMResult PMREMGenerator::GenerateEnvironmentMaps(Ref<Texture2D> hdr)
	{
		auto environmentTextures = InitializeTextures();

		Material EquirectangularMaterial = Material(PMREM_EQUIRECTANGULAR);
		Material ConvolutionMaterial = Material(PMREM_CONVOLUTION);
		Material PrefilterMaterial = Material(PMREM_PREFILTER);

		EquirectangularMaterial.SetTexture("equirectangularMap", TextureBinding(hdr));
		EquirectangularMaterial.SetTexture("imgOutput", TextureBinding(environmentTextures.Environment, 0, 0));
		EquirectangularMaterial.SetParam("u_width", MaterialParam(mSettings.EnvironmentMapSize));
		EquirectangularMaterial.SetParam("u_height", MaterialParam(mSettings.EnvironmentMapSize));

		ConvolutionMaterial.SetTexture("environmentMap", TextureBinding(environmentTextures.Environment));
		ConvolutionMaterial.SetTexture("irradianceMap", TextureBinding(environmentTextures.Irradiance, 0, 0));
		ConvolutionMaterial.SetParam("u_width", MaterialParam(mSettings.IrradianceSize));
		ConvolutionMaterial.SetParam("u_height", MaterialParam(mSettings.IrradianceSize));

		PrefilterMaterial.SetTexture("environmentMap", TextureBinding(environmentTextures.Environment));
		PrefilterMaterial.SetParam("u_envResolution", MaterialParam(mSettings.EnvironmentMapSize));

		Graph graph;
		auto &pass = graph.AddPass("Generate PMREM Maps", EPassType::OffScreen);

		// Phase 0 : equirectangular -> cubemap
		pass.BeginPhase(EPhaseType::Compute);
		pass.UseTexture(hdr, EImageUsage::ComputeSampled);
		pass.UseTexture(environmentTextures.Environment, EImageUsage::ComputeStorageWrite, {0, 1, 0, 6});
		pass.Emplace<CmdBindMaterial>()(&EquirectangularMaterial);
		pass.Emplace<CmdDispatch>()((mSettings.EnvironmentMapSize + 7) / 8, (mSettings.EnvironmentMapSize + 7) / 8, 6);
		pass.EndPhase();

		// Phase 1 : generate mipmaps for environment
		pass.BeginPhase(EPhaseType::Transfer);
		pass.Emplace<CmdGenerateMipMaps>()(environmentTextures.Environment->GetTextureID());
		pass.EndPhase();

		// Phase 2 : Irradiance convolution
		pass.BeginPhase(EPhaseType::Compute);
		pass.UseTexture(environmentTextures.Environment, EImageUsage::ComputeSampled);
		pass.UseTexture(environmentTextures.Irradiance, EImageUsage::ComputeStorageWrite, {0, 1, 0, 6});
		pass.Emplace<CmdBindMaterial>()(&ConvolutionMaterial);
		pass.Emplace<CmdDispatch>()((mSettings.EnvironmentMapSize + 7) / 8, (mSettings.EnvironmentMapSize + 7) / 8, 1);
		pass.EndPhase();

		// Phase 3 - N: Prefilter Specular Mip Chain

		for (uint32_t mip = 0; mip < mSettings.PrefilterMipLevels; mip++)
		{
			uint32_t s = mSettings.PrefilterMapSize >> mip;
			if (s == 0)
				s = 1;

			float roughness = (float)mip / (float)(mSettings.PrefilterMipLevels - 1);

			PrefilterMaterial.SetTexture("imgOutput", TextureBinding(environmentTextures.PreFilter, mip))
				.SetParam("u_roughness", MaterialParam(roughness))
				.SetParam("u_mip_level", MaterialParam(int32_t(mip)))
				.SetParam("u_width", MaterialParam(s))
				.SetParam("u_height", MaterialParam(s));

			pass.BeginPhase(EPhaseType::Compute);
			pass.UseTexture(environmentTextures.Environment, EImageUsage::ComputeSampled);
			pass.UseTexture(environmentTextures.PreFilter, EImageUsage::ComputeStorageWrite, {mip, 1, 0, 6});
			pass.Emplace<CmdBindMaterial>()(&PrefilterMaterial);
			pass.Emplace<CmdDispatch>()((s + 7) / 8, (s + 7) / 8, 6);
			pass.EndPhase();
		}

		Renderer::Get().ExecuteGraph(graph);

		return environmentTextures;
	}

	PMREMResult PMREMGenerator::InitializeTextures()
	{
		PMREMResult environmentTextures = []() -> PMREMResult
		{
			FTextureCreateInfo cubeInfo{};
			cubeInfo.Format = EFormat::RGBA32F;
			cubeInfo.WrapMode = EWrapMode::CLAMP_TO_EDGE;
			cubeInfo.MinFilter = EMinFilter::LINEAR;
			cubeInfo.ArrayLayers = 6;
			cubeInfo.MipLevels = mSettings.PrefilterMipLevels;
			cubeInfo.DebugName = "EnvironmentCube";
			cubeInfo.Roles |= ETextureRole::ComputeWrite;
			auto Environment = CreateRef<TextureCube>(mSettings.EnvironmentMapSize, cubeInfo);

			FTextureCreateInfo convolutionInfo{};
			convolutionInfo.Format = EFormat::RGBA32F;
			convolutionInfo.WrapMode = EWrapMode::CLAMP_TO_EDGE;
			convolutionInfo.MinFilter = EMinFilter::LINEAR;
			convolutionInfo.Roles |= ETextureRole::ComputeWrite;
			convolutionInfo.DebugName = "Irradiance";
			auto Irradiance = CreateRef<TextureCube>(mSettings.IrradianceSize, convolutionInfo);

			FTextureCreateInfo preFilteredInfo{};
			preFilteredInfo.Format = EFormat::RGBA16F;
			preFilteredInfo.WrapMode = EWrapMode::CLAMP_TO_EDGE;
			preFilteredInfo.MinFilter = EMinFilter::MIPMAP_LINEAR;
			preFilteredInfo.MagFilter = EMagFilter::LINEAR;
			preFilteredInfo.MipLevels = mSettings.PrefilterMipLevels;
			preFilteredInfo.Roles |= ETextureRole::ComputeWrite;
			preFilteredInfo.DebugName = "PreFilterEnvironment";
			auto PreFilter = CreateRef<TextureCube>(mSettings.PrefilterMapSize, preFilteredInfo);

			return {.Environment = Environment, .Irradiance = Irradiance, .PreFilter = PreFilter};
		}();

		return environmentTextures;
	}

	Ref<Texture2D> BRDFLUTGenerator::GenerateBRDFLUTMap(uint32_t size)
	{
		Material material = Material(PMREM_BRDFLUT);

		FTextureCreateInfo brdfLUTInfo{};
		brdfLUTInfo.Format = EFormat::RG16F;
		brdfLUTInfo.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		brdfLUTInfo.MagFilter = EMagFilter::NEAREST;
		brdfLUTInfo.MinFilter = EMinFilter::NEAREST;
		brdfLUTInfo.Roles |= ETextureRole::ComputeWrite;
		brdfLUTInfo.DebugName = "BRDFLUT Texture";

		auto brdfLUT = CreateRef<Texture2D>(glm::vec2{size, size}, brdfLUTInfo);

		material.SetTexture("brdfLutTexture", TextureBinding(brdfLUT));

		Graph graph;
		auto &pass = graph.AddPass("Generate BRDFLut", EPassType::OffScreen);
		pass.BeginPhase(EPhaseType::Compute);
		pass.UseTexture(brdfLUT, EImageUsage::ComputeStorageWrite);
		pass.Emplace<CmdBindMaterial>()(&material);
		pass.Emplace<CmdDispatch>()(size / 8, size / 8, 1);
		pass.EndPhase();
		Renderer::Get().ExecuteGraph(graph);

		return brdfLUT;
	}

} // namespace BHive
