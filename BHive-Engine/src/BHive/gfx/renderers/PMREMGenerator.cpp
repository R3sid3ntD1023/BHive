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
			auto equirrectangularPipeline = Pipeline::Create();
			auto state = Pipeline::ComputePipelineState{};
			state.ShaderProgram = EquirectangularShader;
			equirrectangularPipeline->Init(state);
			mEquirectangularMat = CreateRef<Material>(equirrectangularPipeline);
		}

		{
			auto IrradianceShader = ShaderManager::Get().Load("Irradiance.glsl");
			auto irradiancePipeline = Pipeline::Create();
			auto state = Pipeline::ComputePipelineState();
			state.ShaderProgram = IrradianceShader;
			irradiancePipeline->Init(state);
			mIrradianceMat = CreateRef<Material>(irradiancePipeline);
		}

		{
			auto PreFilterEnvironmentShader = ShaderManager::Get().Load("PrefilterEnvironment.glsl");
			auto preFilterPipeline = Pipeline::Create();
			Pipeline::ComputePipelineState state{};
			state.ShaderProgram = PreFilterEnvironmentShader;
			preFilterPipeline->Init(state);
			//mPreFilterEnironmentMat = CreateRef<Material>(preFilterPipeline);

			mPreFilterEnironmentMats.reserve(mSettings.PrefilterMipLevels);
			for (uint32_t i = 0; i < mSettings.PrefilterMipLevels; i++)
				mPreFilterEnironmentMats.push_back(CreateRef<Material>(preFilterPipeline));
		}

		{

			auto BRDFLUTShader = ShaderManager::Get().Load("BRDFLut.glsl");
			auto brdfLUTPipeline = Pipeline::Create();
			Pipeline::ComputePipelineState state{};
			state.ShaderProgram = BRDFLUTShader;
			brdfLUTPipeline->Init(state);
			mBRDFLUTMat = CreateRef<Material>(brdfLUTPipeline);
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
		mEquirectangularMat->SetTexture("equirectangularMap", mEnvironment);

		RenderCommand::AddComputePass(
			"Create CubeMap Pass",
			[=](auto &pass)
			{
				mEquirectangularMat->SetTexture("imgOutput", mEnvironmentCube);
				mEquirectangularMat->Set("u_width", mSettings.EnvironmentMapSize);
				mEquirectangularMat->Set("u_height", mSettings.EnvironmentMapSize);
				mEquirectangularMat->Submit();
				RenderCommand::Dispatch({(mSettings.EnvironmentMapSize + 7) / 8, (mSettings.EnvironmentMapSize + 7) / 8, 6});
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
		mIrradianceMat->SetTexture("environmentMap", mEnvironmentCube);

		RenderCommand::AddComputePass(
			"Create Irradiance Map",
			[=](auto &pass)
			{
				mIrradianceMat->SetTexture("irradianceMap", mIrradiance);		
				mIrradianceMat->Set("u_width", mSettings.IrradianceSize);
				mIrradianceMat->Set("u_height", mSettings.IrradianceSize);
				mIrradianceMat->Submit();
				RenderCommand::Dispatch({(mSettings.EnvironmentMapSize + 7) / 8, (mSettings.EnvironmentMapSize + 7) / 8, 1});
				pass.Images.push_back({mIrradiance, 0, 1, 0, 6, EImageAccess::WRITE});
			});
	}

	void PMREMGenerator::CreatePreFilteredEnvironmentMap()
	{
		//mPreFilterEnironmentMat->SetTexture("environmentMap", mEnvironmentCube);

		for (uint32_t mip = 0; mip < mSettings.PrefilterMipLevels; mip++)
		{
			
			

			uint32_t s = mSettings.PrefilterMapSize >> mip;
			if (s == 0)
				s = 1;
			LOG_INFO("PMREM pass: mip={} size={}x{}", mip, s, s);

			float roughness = (float)mip / (float)(mSettings.PrefilterMipLevels - 1);			
			auto mat = mPreFilterEnironmentMats[mip];

			RenderCommand::AddComputePass("PreFilterEnvironmentPass", [=](auto& pass) {
					
				LOG_INFO("PreFilter pass lambda: binding imgOutput mip={}", mip);

				mat->SetTexture("environmentMap", mEnvironmentCube);
				mat->SetTexture("imgOutput", mPreFilteredEnvironment, mip);
				mat->Set("u_envResolution", mSettings.EnvironmentMapSize);
				mat->Set("u_roughness", roughness);
				mat->Set("u_mip_level", int32_t(mip));
				mat->Set("u_width", s);
				mat->Set("u_height", s);
				mat->Submit();

				RenderCommand::Dispatch({(s + 7) / 8, (s + 7) / 8, 6});

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
		RenderCommand::AddComputePass("BrdfLUTPass", [=](auto& pass) {
				mBRDFLUTMat->SetTexture("brdfLutTexture", mBRDFLUT);
				mBRDFLUTMat->Submit();
				RenderCommand::Dispatch({mSettings.BrdfLutSize / 8, mSettings.BrdfLutSize / 8, 1});
				pass.Images.push_back({mBRDFLUT, 0, 1});
			});
	
	}
} // namespace BHive
