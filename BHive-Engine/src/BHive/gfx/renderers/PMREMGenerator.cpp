#include "gfx/Framebuffer.h"
#include "gfx/RenderCommand.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "PMREMGenerator.h"
#include "gfx/material/Material.h"
#include "gfx/Pipeline.h"

namespace BHive
{
	PMREMGenerator::PMREMGenerator(const PMREMSettings &settings)
		: mSettings(settings)
	{}

	void PMREMGenerator::Initialize()
	{
		{
			FTextureCreateInfo create_info{};
			create_info.Format = EFormat::RGBA32F;
			create_info.WrapMode = EWrapMode::CLAMP_TO_EDGE;
			create_info.MinFilter = EMinFilter::LINEAR;
			create_info.ArrayLayers = 6;
			create_info.MipLevels = mSettings.PrefilterMipLevels;
			create_info.DebugName = "EnvironmentCube";
			create_info.Roles |= ETextureRole::ComputeWrite;
			mEnvironmentCube = TextureCube::Create(mSettings.EnvironmentMapSize, create_info);
		}
		
		{
			FTextureCreateInfo create_info{};
			create_info.Format = EFormat::RGBA32F;
			create_info.WrapMode = EWrapMode::CLAMP_TO_EDGE;
			create_info.MinFilter = EMinFilter::LINEAR;
			create_info.Roles |= ETextureRole::ComputeWrite;
			create_info.DebugName = "Irradiance";
			mIrradiance = TextureCube::Create(mSettings.IrradianceSize, create_info);
		}

		{
			FTextureCreateInfo create_info{};
			create_info.Format = EFormat::RGBA16F;
			create_info.WrapMode = EWrapMode::CLAMP_TO_EDGE;
			create_info.MinFilter = EMinFilter::MIPMAP_LINEAR;
			create_info.MagFilter = EMagFilter::LINEAR;
			create_info.MipLevels = mSettings.PrefilterMipLevels;
			create_info.Roles |= ETextureRole::ComputeWrite;
			create_info.DebugName = "PreFilterEnvironment";
			mPreFilteredEnvironment = TextureCube::Create(mSettings.PrefilterMapSize, create_info);
		}

		{
			FTextureCreateInfo brdfLUTCreateInfo{};
			brdfLUTCreateInfo.Format = EFormat::RG16F;
			brdfLUTCreateInfo.WrapMode = EWrapMode::CLAMP_TO_EDGE;
			brdfLUTCreateInfo.MagFilter = EMagFilter::NEAREST;
			brdfLUTCreateInfo.MinFilter = EMinFilter::NEAREST;
			brdfLUTCreateInfo.Roles |= ETextureRole::ComputeWrite;
			brdfLUTCreateInfo.DebugName = "BRDFLUT Texture";

			mBRDFLUT = Texture2D::Create({mSettings.BrdfLutSize, mSettings.BrdfLutSize}, brdfLUTCreateInfo);
		}

		{
			auto EquirectangularShader = ShaderManager::Get().Load("Equirectangular.glsl");
			mEquirectangularPipeline = Pipeline::Create();
			Pipeline::ComputePipelineState state{};
			state.ShaderProgram = EquirectangularShader;
			mEquirectangularPipeline->Init(state);
		}

		{
			auto IrradianceShader = ShaderManager::Get().Load("Irradiance.glsl");
			mIrradiancePipeline = Pipeline::Create();
			Pipeline::ComputePipelineState state{};
			state.ShaderProgram = IrradianceShader;
			mIrradiancePipeline->Init(state);
		}

		{
			auto PreFilterEnvironmentShader = ShaderManager::Get().Load("PrefilterEnvironment.glsl");
			mPreFilterEnironmentPipeline = Pipeline::Create();
			Pipeline::ComputePipelineState state{};
			state.ShaderProgram = PreFilterEnvironmentShader;
			mPreFilterEnironmentPipeline->Init(state);
		}

		{

			auto BRDFLUTShader = ShaderManager::Get().Load("BRDFLut.glsl");
			mBRDFLUTPipeline = Pipeline::Create();
			Pipeline::ComputePipelineState state{};
			state.ShaderProgram = BRDFLUTShader;
			mBRDFLUTPipeline->Init(state);
		}

	}

	void PMREMGenerator::SetEnvironmentMap(const Ref<Texture> &texture)
	{
		mEnvironment = texture;
		CreateEnvironmentCubeMap();
		CreatePreFilteredEnvironmentMap();
		CreateIrradianceMap();
		CreateBRDFLUTMap();
	}

	const Ref<Texture> &PMREMGenerator::GetIrradianceTexture() const
	{
		return mIrradiance;
	}

	const Ref<Texture> &PMREMGenerator::GetPreFilteredEnvironmentTexture() const
	{
		return mPreFilteredEnvironment;
	}

	const Ref<Texture> &PMREMGenerator::GetEnvironmentCubeTexture() const
	{
		return mEnvironmentCube;
	}

	const Ref<Texture> &PMREMGenerator::GetBDRFLUT() const
	{
		return mBRDFLUT;
	}

	void PMREMGenerator::CreateEnvironmentCubeMap()
	{	
		RenderCommand::AddComputePass(
			"Create CubeMap Pass",
			mEquirectangularPipeline, {(mSettings.EnvironmentMapSize + 7) / 8, (mSettings.EnvironmentMapSize + 7) / 8, 6},
			[=](FComputeBindings& b, FRenderGraphPass &pass)
			{
				b.SampledImage("equirectangularMap", mEnvironment);
				b.StorageImage("imgOutput", mEnvironmentCube);
				b.Set("u_width", mSettings.EnvironmentMapSize);
				b.Set("u_height", mSettings.EnvironmentMapSize);

				pass.Images.push_back({mEnvironmentCube, 0, 1, 0, 6, EImageAccess::WRITE});
			});

		RenderCommand::AddTransferPass(
			"Generate Mips",
			[=](auto& pass) { 
				mEnvironmentCube->GenerateMips();
				pass.Images.push_back({mEnvironmentCube, 0, mSettings.PrefilterMipLevels, 0, 6, EImageAccess::WRITE});
			});

	}

	void PMREMGenerator::CreateIrradianceMap()
	{
		RenderCommand::AddComputePass(
			"Create Irradiance Map", mIrradiancePipeline, {(mSettings.EnvironmentMapSize + 7) / 8, (mSettings.EnvironmentMapSize + 7) / 8, 1},
			[=](FComputeBindings &b, FRenderGraphPass &pass)
			{
				b.SampledImage("environmentMap", mEnvironmentCube);
				b.StorageImage("irradianceMap", mIrradiance);		
				b.Set("u_width", mSettings.IrradianceSize);
				b.Set("u_height", mSettings.IrradianceSize);

				pass.Images.push_back({mIrradiance, 0, 1, 0, 6, EImageAccess::WRITE});
			});
	}

	void PMREMGenerator::CreatePreFilteredEnvironmentMap()
	{
		for (uint32_t mip = 0; mip < mSettings.PrefilterMipLevels; mip++)
		{
			uint32_t s = mSettings.PrefilterMapSize >> mip;
			if (s == 0)
				s = 1;

			float roughness = (float)mip / (float)(mSettings.PrefilterMipLevels - 1);			

			RenderCommand::AddComputePass(
				"PreFilterEnvironmentPass",
				mPreFilterEnironmentPipeline, {(s + 7) / 8, (s + 7) / 8, 6},
				[=](FComputeBindings &b, FRenderGraphPass &pass)
				{
					
				LOG_INFO("PreFilter pass lambda: binding imgOutput mip={}", mip);

				b.SampledImage("environmentMap", mEnvironmentCube);
				b.StorageImage("imgOutput", mPreFilteredEnvironment, mip);
				b.Set("u_envResolution", mSettings.EnvironmentMapSize);
				b.Set("u_roughness", roughness);
				b.Set("u_mip_level", int32_t(mip));
				b.Set("u_width", s);
				b.Set("u_height", s);

				FPassImage image{};
				image.Texture = mPreFilteredEnvironment;
				image.Access = EImageAccess::WRITE;
				image.BaseLayer = 0;
				image.LayerCount = 6;
				image.BaseMip = mip;
				image.LevelCount = 1;
				pass.Images.push_back(image);
			});
		}

	}

	void PMREMGenerator::CreateBRDFLUTMap()
	{
		RenderCommand::AddComputePass(
			"BrdfLUTPass", 
			mBRDFLUTPipeline, {mSettings.BrdfLutSize / 8, mSettings.BrdfLutSize / 8, 1},
			[=](FComputeBindings &b, FRenderGraphPass &pass)
			{
				b.StorageImage("brdfLutTexture", mBRDFLUT);
				pass.Images.push_back({mBRDFLUT, 0, 1});
			});
	
	}
} // namespace BHive
