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

	PMREMResult PMREMGenerator::GenerateEnvironmentMaps(Texture2DPtr hdr)
	{
		auto environmentTextures = InitializeTextures();

		static MaterialPtr EquirectangularMaterial = MaterialFactory::Create(PMREM_EQUIRECTANGULAR);
		static MaterialPtr ConvolutionMaterial = MaterialFactory::Create(PMREM_CONVOLUTION);
		static MaterialPtr PreFilterMaterial = MaterialFactory::Create(PMREM_PREFILTER);

		auto equirectangularMaterial = EquirectangularMaterial.As<Material>();
		auto convolutionMaterial = ConvolutionMaterial.As<Material>();
		auto prefilterMaterial = PreFilterMaterial.As<Material>();

		equirectangularMaterial->SetTexture("equirectangularMap", FTextureBinding(hdr));
		equirectangularMaterial->SetTexture("imgOutput", FTextureBinding(environmentTextures.Environment, 0, 0));
		equirectangularMaterial->SetParam("u_width", MaterialParam(mSettings.EnvironmentMapSize));
		equirectangularMaterial->SetParam("u_height", MaterialParam(mSettings.EnvironmentMapSize));

		convolutionMaterial->SetTexture("environmentMap", FTextureBinding(environmentTextures.Environment));
		convolutionMaterial->SetTexture("irradianceMap", FTextureBinding(environmentTextures.Irradiance, 0, 0));
		convolutionMaterial->SetParam("u_width", MaterialParam(mSettings.IrradianceSize));
		convolutionMaterial->SetParam("u_height", MaterialParam(mSettings.IrradianceSize));

		prefilterMaterial->SetTexture("environmentMap", FTextureBinding(environmentTextures.Environment));
		prefilterMaterial->SetParam("u_envResolution", MaterialParam(mSettings.EnvironmentMapSize));

		RenderGraph graph;
		auto &pass = graph.AddPass("Generate PMREM Maps", EPassType::OffScreen);

		// Phase 0 : equirectangular -> cubemap
		pass.BeginPhase("Convert 2D -> cube", EPhaseType::Compute);
		pass.UseTexture(hdr, EImageUsage::ComputeSampled);
		pass.UseTexture(environmentTextures.Environment, EImageUsage::ComputeStorageWrite, {0, 1, 0, 6});
		pass.Emplace<CmdBindMaterial>()(equirectangularMaterial);
		pass.Emplace<CmdDispatch>()((mSettings.EnvironmentMapSize + 7) / 8, (mSettings.EnvironmentMapSize + 7) / 8, 6);
		pass.EndPhase();

		// Phase 1 : generate mipmaps for environment
		pass.BeginPhase("Gen Mips for Environment Cube", EPhaseType::Transfer);
		pass.Emplace<CmdGenerateMipMaps>()(environmentTextures.Environment);
		pass.EndPhase();

		// Phase 2 : Irradiance convolution
		pass.BeginPhase("Convolution", EPhaseType::Compute);
		pass.UseTexture(environmentTextures.Environment, EImageUsage::ComputeSampled);
		pass.UseTexture(environmentTextures.Irradiance, EImageUsage::ComputeStorageWrite, {0, 1, 0, 6});
		pass.Emplace<CmdBindMaterial>()(convolutionMaterial);
		pass.Emplace<CmdDispatch>()((mSettings.EnvironmentMapSize + 7) / 8, (mSettings.EnvironmentMapSize + 7) / 8, 1);
		pass.EndPhase();

		// Phase 3 - N: Prefilter Specular Mip Chain

		for (uint32_t mip = 0; mip < mSettings.PrefilterMipLevels; mip++)
		{
			uint32_t s = mSettings.PrefilterMapSize >> mip;
			if (s == 0)
				s = 1;

			float roughness = (float)mip / (float)(mSettings.PrefilterMipLevels - 1);

			prefilterMaterial->SetTexture("imgOutput", FTextureBinding(environmentTextures.PreFilter, mip))
				.SetParam("u_roughness", MaterialParam(roughness))
				.SetParam("u_mip_level", MaterialParam(int32_t(mip)))
				.SetParam("u_width", MaterialParam(s))
				.SetParam("u_height", MaterialParam(s));

			pass.BeginPhase(std::format("Prefiltering Mip {}", mip), EPhaseType::Compute);
			pass.UseTexture(environmentTextures.Environment, EImageUsage::ComputeSampled);
			pass.UseTexture(environmentTextures.PreFilter, EImageUsage::ComputeStorageWrite, {mip, 1, 0, 6});
			pass.Emplace<CmdBindMaterial>()(prefilterMaterial);
			pass.Emplace<CmdDispatch>()((s + 7) / 8, (s + 7) / 8, 6);
			pass.EndPhase();
		}

		Renderer::Get().ExecuteGraph(graph);

		return environmentTextures;
	}

	PMREMResult PMREMGenerator::InitializeTextures()
	{

		PMREMResult environmentTextures{};

		FTextureCreateInfo cubeInfo{};
		cubeInfo.Format = EFormat::RGBA32F;
		cubeInfo.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		cubeInfo.MinFilter = EMinFilter::LINEAR;
		cubeInfo.ArrayLayers = 6;
		cubeInfo.MipLevels = mSettings.PrefilterMipLevels;
		cubeInfo.DebugName = "EnvironmentCube";
		cubeInfo.Roles |= ETextureRole::ComputeWrite;
		environmentTextures.Environment = TextureFactory::CreateCube(mSettings.EnvironmentMapSize, cubeInfo);

		FTextureCreateInfo convolutionInfo{};
		convolutionInfo.Format = EFormat::RGBA32F;
		convolutionInfo.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		convolutionInfo.MinFilter = EMinFilter::LINEAR;
		convolutionInfo.Roles |= ETextureRole::ComputeWrite;
		convolutionInfo.DebugName = "Irradiance";
		environmentTextures.Irradiance = TextureFactory::CreateCube(mSettings.IrradianceSize, convolutionInfo);

		FTextureCreateInfo preFilteredInfo{};
		preFilteredInfo.Format = EFormat::RGBA16F;
		preFilteredInfo.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		preFilteredInfo.MinFilter = EMinFilter::MIPMAP_LINEAR;
		preFilteredInfo.MagFilter = EMagFilter::LINEAR;
		preFilteredInfo.MipLevels = mSettings.PrefilterMipLevels;
		preFilteredInfo.Roles |= ETextureRole::ComputeWrite;
		preFilteredInfo.DebugName = "PreFilterEnvironment";
		environmentTextures.PreFilter = TextureFactory::CreateCube(mSettings.PrefilterMapSize, preFilteredInfo);

		return environmentTextures;
	}

	Texture2DPtr BRDFLUTGenerator::GenerateBRDFLUTMap(uint32_t size)
	{
		static MaterialPtr BRDFLUTMaterial = MaterialFactory::Create(PMREM_BRDFLUT);

		auto material = BRDFLUTMaterial.As<Material>();
		FTextureCreateInfo brdfLUTInfo{};
		brdfLUTInfo.Format = EFormat::RG16F;
		brdfLUTInfo.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		brdfLUTInfo.MagFilter = EMagFilter::NEAREST;
		brdfLUTInfo.MinFilter = EMinFilter::NEAREST;
		brdfLUTInfo.Roles |= ETextureRole::ComputeWrite;
		brdfLUTInfo.DebugName = "BRDFLUT Texture";

		auto brdfLUT = TextureFactory::Create2D({size, size}, brdfLUTInfo);

		material->SetTexture("brdfLutTexture", FTextureBinding(brdfLUT));

		RenderGraph graph;
		auto &pass = graph.AddPass("Generate BRDFLut", EPassType::OffScreen);
		pass.BeginPhase(EPhaseType::Compute);
		pass.Emplace<CmdBindMaterial>()(material);
		pass.Emplace<CmdDispatch>()(size / 8, size / 8, 1);
		pass.EndPhase();
		Renderer::Get().ExecuteGraph(graph);

		return brdfLUT;
	}

} // namespace BHive
