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

	void PMREMGenerator::Initialize(const PMREMSettings &settings)
	{
		mSettings = settings;
		InitializeTextures();
	}

	PMREMResult PMREMGenerator::GenerateEnvironmentMaps(const Ref<Texture2D> &hdr)
	{
		mInput = hdr;

		auto conversionBindings = FComputeBindings(PMREM_EQUIRECTANGULAR);
		conversionBindings.SetTexture("equirectangularMap", FTextureBinding(mInput))
			.SetTexture("imgOutput", FTextureBinding(mEnvironmentTextures.Environment, 0, 0))
			.SetParam("u_width", MaterialParam(mSettings.EnvironmentMapSize))
			.SetParam("u_height", MaterialParam(mSettings.EnvironmentMapSize));

		auto convolutionBindings = FComputeBindings(PMREM_CONVOLUTION);
		convolutionBindings.SetTexture("environmentMap", FTextureBinding(mEnvironmentTextures.Environment))
			.SetTexture("irradianceMap", FTextureBinding(mEnvironmentTextures.Irradiance, 0, 0))
			.SetParam("u_width", MaterialParam(mSettings.IrradianceSize))
			.SetParam("u_height", MaterialParam(mSettings.IrradianceSize));

		auto prefilterBindings = FComputeBindings(PMREM_PREFILTER);
		prefilterBindings.SetTexture("environmentMap", FTextureBinding(mEnvironmentTextures.Environment)).SetParam("u_envResolution", MaterialParam(mSettings.EnvironmentMapSize));

		RenderGraph graph;
		auto &pass = graph.AddPass("Generate PMREM Maps", EPassType::OffScreen);

		// Phase 0 : equirectangular -> cubemap
		pass.BeginPhase("Convert 2D -> cube", EPhaseType::Compute);
		pass.Push(mInput, EImageUsage::ComputeSampled);
		pass.Push(mEnvironmentTextures.Environment, EImageUsage::ComputeStorageWrite, {0, 1, 0, 6});
		pass.Emplace<CmdBindMaterial>()(&conversionBindings);
		pass.Emplace<CmdDispatch>()((mSettings.EnvironmentMapSize + 7) / 8, (mSettings.EnvironmentMapSize + 7) / 8, 6);
		pass.EndPhase();

		// Phase 1 : generate mipmaps for environment
		pass.BeginPhase("Gen Mips for Environment Cube", EPhaseType::Transfer);
		pass.Emplace<CmdGenerateMipMaps>()(mEnvironmentTextures.Environment);
		pass.EndPhase();

		// Phase 2 : Irradiance convolution
		pass.BeginPhase("Convolution", EPhaseType::Compute);
		pass.Push(mEnvironmentTextures.Environment, EImageUsage::ComputeSampled);
		pass.Push(mEnvironmentTextures.Irradiance, EImageUsage::ComputeStorageWrite, {0, 1, 0, 6});
		pass.Emplace<CmdBindMaterial>()(&convolutionBindings);
		pass.Emplace<CmdDispatch>()((mSettings.EnvironmentMapSize + 7) / 8, (mSettings.EnvironmentMapSize + 7) / 8, 1);
		pass.EndPhase();

		// Phase 3 - N: Prefilter Specular Mip Chain

		for (uint32_t mip = 0; mip < mSettings.PrefilterMipLevels; mip++)
		{
			uint32_t s = mSettings.PrefilterMapSize >> mip;
			if (s == 0)
				s = 1;

			float roughness = (float)mip / (float)(mSettings.PrefilterMipLevels - 1);

			prefilterBindings.SetTexture("imgOutput", FTextureBinding(mEnvironmentTextures.PreFilter, mip))
				.SetParam("u_roughness", MaterialParam(roughness))
				.SetParam("u_mip_level", MaterialParam(int32_t(mip)))
				.SetParam("u_width", MaterialParam(s))
				.SetParam("u_height", MaterialParam(s));

			pass.BeginPhase(std::format("Prefiltering Mip {}", mip), EPhaseType::Compute);
			pass.Push(mEnvironmentTextures.Environment, EImageUsage::ComputeSampled);
			pass.Push(mEnvironmentTextures.PreFilter, EImageUsage::ComputeStorageWrite, {mip, 1, 0, 6});
			pass.Emplace<CmdBindMaterial>()(&prefilterBindings);
			pass.Emplace<CmdDispatch>()((s + 7) / 8, (s + 7) / 8, 6);
			pass.EndPhase();
		}

		Renderer::Get().ExecuteGraph(graph);

		return mEnvironmentTextures;
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

	Ref<Texture2D> BRDFLUTGenerator::GenerateBRDFLUTMap(uint32_t size)
	{
		FTextureCreateInfo brdfLUTInfo{};
		brdfLUTInfo.Format = EFormat::RG16F;
		brdfLUTInfo.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		brdfLUTInfo.MagFilter = EMagFilter::NEAREST;
		brdfLUTInfo.MinFilter = EMinFilter::NEAREST;
		brdfLUTInfo.Roles |= ETextureRole::ComputeWrite;
		brdfLUTInfo.DebugName = "BRDFLUT Texture";

		auto brdfLUT = Texture2D::Create({size, size}, brdfLUTInfo);

		auto bindings = FComputeBindings(PMREM_BRDFLUT);
		bindings.SetTexture("brdfLutTexture", FTextureBinding(brdfLUT));

		RenderGraph graph;
		auto &pass = graph.AddPass("Generate BRDFLut", EPassType::OffScreen);
		pass.BeginPhase(EPhaseType::Compute);
		pass.Emplace<CmdBindMaterial>()(&bindings);
		pass.Emplace<CmdDispatch>()(size / 8, size / 8, 1);
		pass.EndPhase();
		Renderer::Get().ExecuteGraph(graph);

		return brdfLUT;
	}

} // namespace BHive
