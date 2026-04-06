#include "gfx/Framebuffer.h"
#include "gfx/RenderCommand.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "PMREMGenerator.h"
#include "gfx/material/Material.h"
#include "gfx/Pipeline.h"

#define ENVIRONMENT_MAP_SIZE 512
#define PREFILTER_MAP_SIZE 128
#define PREFILTER_MIP_LEVELS 5
#define PREFILTER_WORK_GROUP_SIZE 8
#define MAX_PREFILTER_SAMPLES 64
#define IRRANDIANCE_CUBEMAP_SIZE 32
#define BRDF_LUT_SIZE 512
#define BRDF_WORK_GROUP_SIZE 8

namespace BHive
{
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
			create_info.MipLevels = 1;
			create_info.DebugName = "EnvironmentCube";
			create_info.Usage |= ETextureUsage::Storage;
			mEnvironmentCube = TextureCube::Create(ENVIRONMENT_MAP_SIZE, create_info);
		}
		
		{
			FTextureCreateInfo create_info{};
			create_info.Format = EFormat::RGBA32F;
			create_info.WrapMode = EWrapMode::CLAMP_TO_EDGE;
			create_info.MinFilter = EMinFilter::LINEAR;
			create_info.Usage |= ETextureUsage::Storage;
			create_info.DebugName = "Irradiance";
			mIrradiance = TextureCube::Create(IRRANDIANCE_CUBEMAP_SIZE, create_info);
		}

		{
			FTextureCreateInfo create_info{};
			create_info.Format = EFormat::RGBA16F;
			create_info.WrapMode = EWrapMode::CLAMP_TO_EDGE;
			create_info.MinFilter = EMinFilter::MIPMAP_LINEAR;
			create_info.MagFilter = EMagFilter::LINEAR;
			create_info.MipLevels = PREFILTER_MIP_LEVELS;
			create_info.Usage |= ETextureUsage::Storage;
			create_info.DebugName = "PreFilterEnvironment";
			mPreFilteredEnvironment = TextureCube::Create(PREFILTER_MAP_SIZE, create_info);
		}

		{
			FTextureCreateInfo brdfLUTCreateInfo{};
			brdfLUTCreateInfo.Format = EFormat::RG16F;
			brdfLUTCreateInfo.WrapMode = EWrapMode::CLAMP_TO_EDGE;
			brdfLUTCreateInfo.MagFilter = EMagFilter::NEAREST;
			brdfLUTCreateInfo.MinFilter = EMinFilter::NEAREST;
			brdfLUTCreateInfo.Usage |= ETextureUsage::Storage;
			brdfLUTCreateInfo.DebugName = "BRDFLUT Texture";

			mBRDFLUT = Texture2D::Create({BRDF_LUT_SIZE, BRDF_LUT_SIZE}, brdfLUTCreateInfo);
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
		//CreateIrradianceMap();
		//CreatePreFilteredEnvironmentMap();
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
		mEquirectangularMat->Set("u_width", ENVIRONMENT_MAP_SIZE);
		mEquirectangularMat->Set("u_height", ENVIRONMENT_MAP_SIZE);

		RenderCommand::AddComputePass(
			"Create CubeMap Pass",
			[=](auto &pass)
			{
				mEquirectangularMat->Submit();
				RenderCommand::Dispatch({(ENVIRONMENT_MAP_SIZE + 7) / 8, (ENVIRONMENT_MAP_SIZE + 7) / 8, 1});
				pass.Images.push_back({mEnvironmentCube, 0, 6});
			});
	}

	void PMREMGenerator::CreateIrradianceMap()
	{
		mIrradianceMat->SetTexture("irradianceMap", mIrradiance);
		mIrradianceMat->SetTexture("environmentMap", mEnvironmentCube);
		mIrradianceMat->Set("u_width", IRRANDIANCE_CUBEMAP_SIZE);
		mIrradianceMat->Set("u_height", IRRANDIANCE_CUBEMAP_SIZE);

		RenderCommand::AddComputePass(
			"Create Irradiance Map",
			[=](auto &pass)
			{
				mIrradianceMat->Submit();
				RenderCommand::Dispatch({(ENVIRONMENT_MAP_SIZE + 7) / 8, (ENVIRONMENT_MAP_SIZE + 7) / 8, 1});
				pass.Images.push_back({mIrradiance, 0, 6});
			});
	}

	void PMREMGenerator::CreatePreFilteredEnvironmentMap()
	{
		mPreFilterEnironmentMat->SetTexture("environmentMap", mEnvironmentCube);
		

		int mip_level = (ENVIRONMENT_MAP_SIZE / PREFILTER_MAP_SIZE) - 1;
		for (int i = 0; i < PREFILTER_MIP_LEVELS; i++)
		{
			unsigned w = PREFILTER_MAP_SIZE * pow(0.5f, i);
			unsigned h = PREFILTER_MAP_SIZE * pow(0.5f, i);

			float roughness = (float)i / (float)(PREFILTER_MIP_LEVELS - 1);

			mPreFilterEnironmentMat->Set("u_roughness", roughness);
			mPreFilterEnironmentMat->Set("u_mip_level", mip_level);
			mPreFilterEnironmentMat->Set("u_width", w);
			mPreFilterEnironmentMat->Set("u_height", h);

			mPreFilterEnironmentMat->SetTexture("imgOutput", mPreFilteredEnvironment, i);

			RenderCommand::AddComputePass("BrdfLUTPass", [=](auto& pass) {
					mPreFilterEnironmentMat->Submit();
					RenderCommand::Dispatch({w / PREFILTER_WORK_GROUP_SIZE, h / PREFILTER_WORK_GROUP_SIZE, 1});

					pass.Images.push_back({mPreFilteredEnvironment, (uint32_t)mip_level, 1});
				});
		}

	}

	void PMREMGenerator::CreateBRDFLUTMap()
	{
		mBRDFLUTMat->SetTexture("brdfLutTexture", mBRDFLUT);

		RenderCommand::AddComputePass("BrdfLUTPass", [=](auto& pass) {
				mBRDFLUTMat->Submit();
				RenderCommand::Dispatch({BRDF_LUT_SIZE / BRDF_WORK_GROUP_SIZE, BRDF_LUT_SIZE / BRDF_WORK_GROUP_SIZE, 1});
				pass.Images.push_back({mBRDFLUT, 0, 1});
			});
	
	}
} // namespace BHive
