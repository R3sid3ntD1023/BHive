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
		if (mInitialized)
			return;

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
			mPreFilterEnironmentMat = CreateRef<Material>(preFilterPipeline);
		}

		{

			auto BRDFLUTShader = ShaderManager::Get().Load("BRDFLut.glsl");
			auto brdfLUTPipeline = Pipeline::Create();
			Pipeline::ComputePipelineState state{};
			state.ShaderProgram = BRDFLUTShader;
			brdfLUTPipeline->Init(state);
			mBRDFLUTMat = CreateRef<Material>(brdfLUTPipeline);
		}

		mInitialized = true;
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

	const Ref<Texture> &PMREMGenerator::GetPreFilteredEnvironmentTetxure() const
	{
		return mPreFilteredEnvironment;
	}

	const Ref<Texture> &PMREMGenerator::GetBDRFLUT() const
	{
		return mBRDFLUT;
	}

	void PMREMGenerator::CreateEnvironmentCubeMap()
	{	
		mEquirectangularMat->SetTexture("imgOutput", mEnvironmentCube);
		mEquirectangularMat->SetTexture("equirectangularMap", mEnvironment);
		mEquirectangularMat->Set("u_width", mSettings.EnvironmentMapSize);
		mEquirectangularMat->Set("u_height", mSettings.EnvironmentMapSize);

		RenderCommand::AddComputePass(
			"Create CubeMap Pass",
			[=](auto &pass)
			{
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
		mIrradianceMat->SetTexture("irradianceMap", mIrradiance);
		mIrradianceMat->SetTexture("environmentMap", mEnvironmentCube);
		mIrradianceMat->Set("u_width", mSettings.IrradianceSize);
		mIrradianceMat->Set("u_height", mSettings.IrradianceSize);

		RenderCommand::AddComputePass(
			"Create Irradiance Map",
			[=](auto &pass)
			{
				mIrradianceMat->Submit();
				RenderCommand::Dispatch({(mSettings.EnvironmentMapSize + 7) / 8, (mSettings.EnvironmentMapSize + 7) / 8, 1});
				pass.Images.push_back({mIrradiance, 0, 1, 0, 6, EImageAccess::WRITE});
			});
	}

	void PMREMGenerator::CreatePreFilteredEnvironmentMap()
	{
		mPreFilterEnironmentMat->SetTexture("environmentMap", mEnvironmentCube);
		mPreFilterEnironmentMat->Set("u_envResolution", mSettings.EnvironmentMapSize);
		
		for (uint32_t mip = 0; mip < mSettings.PrefilterMipLevels; mip++)
		{
			uint32_t s = mSettings.PrefilterMapSize >> mip;
			if (s == 0)
				s = 1;

			float roughness = (float)mip / (float)(mSettings.PrefilterMipLevels - 1);

			RenderCommand::AddComputePass("PreFilterEnvironmentPass", [=](auto& pass) {
					mPreFilterEnironmentMat->Set("u_roughness", roughness);
					mPreFilterEnironmentMat->Set("u_mip_level", (int)mip);
					mPreFilterEnironmentMat->Set("u_width", s);
					mPreFilterEnironmentMat->Set("u_height", s);
					mPreFilterEnironmentMat->SetTexture("imgOutput", mPreFilteredEnvironment, mip);
					mPreFilterEnironmentMat->Submit();
					RenderCommand::Dispatch({(s + 7) / 8, (s +7) / 8, 6});

					pass.Images.push_back({mPreFilteredEnvironment, mip, 1, 0 , 6, EImageAccess::WRITE});
				});
		}

	}

	void PMREMGenerator::CreateBRDFLUTMap()
	{
		mBRDFLUTMat->SetTexture("brdfLutTexture", mBRDFLUT);

		RenderCommand::AddComputePass("BrdfLUTPass", [=](auto& pass) {
				mBRDFLUTMat->Submit();
				RenderCommand::Dispatch({mSettings.BrdfLutSize / 8, mSettings.BrdfLutSize / 8, 1});
				pass.Images.push_back({mBRDFLUT, 0, 1});
			});
	
	}
} // namespace BHive
