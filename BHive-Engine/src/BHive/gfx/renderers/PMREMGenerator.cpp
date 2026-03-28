#include "gfx/Framebuffer.h"
#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "gfx/mesh/primitives/Cube.h"
#include "PMREMGenerator.h"
#include "Renderer.h"
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

		mEnvironmentCapture = CreateRef<RenderTargetCube>(ENVIRONMENT_MAP_SIZE, EFormat::RGBA32F);

		mIrradianceCapture = CreateRef<RenderTargetCube>(IRRANDIANCE_CUBEMAP_SIZE, EFormat::RGBA32F);

		FTextureCreateInfo pre_filter_specification;
		pre_filter_specification.Format = EFormat::RGBA16F;
		pre_filter_specification.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		pre_filter_specification.MinFilter = EMinFilter::MIPMAP_LINEAR;
		pre_filter_specification.MagFilter = EMagFilter::LINEAR;
		pre_filter_specification.Levels = PREFILTER_MIP_LEVELS;
		pre_filter_specification.Usage |= ETextureUsage::Storage;
		pre_filter_specification.DebugName = "PreFilterEnvironment Texture";

		mPreFilteredEnvironmentTexture = TextureCube::Create(PREFILTER_MAP_SIZE, pre_filter_specification);

		FTextureCreateInfo brdfLUTCreateInfo{};
		brdfLUTCreateInfo.Format = EFormat::R16F;
		brdfLUTCreateInfo.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		brdfLUTCreateInfo.MagFilter = EMagFilter::NEAREST;
		brdfLUTCreateInfo.MinFilter = EMinFilter::NEAREST;
		brdfLUTCreateInfo.Usage |= ETextureUsage::Storage;
		brdfLUTCreateInfo.DebugName = "BRDFLUT Texture";

		mBRDFLUTTexture = Texture2D::Create({BRDF_LUT_SIZE, BRDF_LUT_SIZE}, brdfLUTCreateInfo);

		mCube = CreateRef<PCube>(2.0f);

		auto EquirectangularShader = ShaderManager::Get().Load("Equirectangular.glsl");
		auto IrradianceShader = ShaderManager::Get().Load("Irradiance.glsl");
		auto PreFilterEnironmentShader = ShaderManager::Get().Load("PrefilterEnvironment.glsl");
		auto BRDFLUTShader = ShaderManager::Get().Load("BRDFLut.glsl");

		auto equirrectangularPipeline = Pipeline::Create();
		auto irradiancePipeline = Pipeline::Create();
		auto preFilterPipeline = Pipeline::Create();
		auto brdfLUTPipeline = Pipeline::Create();

		auto state = Pipeline::GetDefaultPipelineState();

		state.ShaderProgram = EquirectangularShader;
		state.ColorAttachmentFormats = {EFormat::RGBA32F};
		state.Raster.CullMode = ECullMode::Front;
		equirrectangularPipeline->Init(state);
		mEquirectangularMat = CreateRef<Material>(equirrectangularPipeline);

		state.ShaderProgram = IrradianceShader;
		irradiancePipeline->Init(state);
		mIrradianceMat = CreateRef<Material>(irradiancePipeline);

		Pipeline::ComputePipelineState compute_state{};
		compute_state.ShaderProgram = PreFilterEnironmentShader;
		preFilterPipeline->Init(compute_state);
		mPreFilterEnironmentMat = CreateRef<Material>(preFilterPipeline);

		compute_state.ShaderProgram = BRDFLUTShader;
		brdfLUTPipeline->Init(compute_state);
		mBRDFLUTMat = CreateRef<Material>(brdfLUTPipeline);

		mInitialized = true;
	}

	void PMREMGenerator::SetEnvironmentMap(const Ref<Texture> &texture)
	{
		mEnvironmentTexture = texture;
		CreateEnvironmentCubeMap();
		CreateIrradianceMap();
		/*CreateBRDFLUTMap();
		CreatePreFilteredEnvironmentMap();*/
	}

	const Ref<Texture> &PMREMGenerator::GetIrradianceTexture() const
	{
		return mIrradianceCapture->GetTargetTexture();
	}

	const Ref<Texture> &PMREMGenerator::GetPreFilteredEnvironmentTetxure() const
	{
		return mPreFilteredEnvironmentTexture;
	}

	const Ref<Texture> &PMREMGenerator::GetBDRFLUT() const
	{
		return mBRDFLUTTexture;
	}

	void PMREMGenerator::CreateEnvironmentCubeMap()
	{	
		mEquirectangularMat->SetTexture("equirectangularMap", mEnvironmentTexture);

		for (int i = 0; i < 6; i++)
		{
			mEnvironmentCapture->Bind(i);
	
			RenderCommand::Clear();
			RenderCommand::SetViewport(0, 0, 512, 512);
			Renderer::Begin();

			Renderer::SubmitCamera(mCubeCamera.GetProjection(), mCubeCamera.GetView({}, i));

			mEquirectangularMat->Submit();
			
			auto &submesh = mCube->GetSubMeshes()[0];
			RenderCommand::DrawElementsBaseVertex(ETopologyMode::Triangles, mCube->GetVertexArray(), submesh.StartVertex, submesh.StartIndex, submesh.IndexCount);

			Renderer::End();

			mEnvironmentCapture->UnBind();
		}
	}

	void PMREMGenerator::CreateIrradianceMap()
	{
		mIrradianceMat->SetTexture("environmentMap", mEnvironmentCapture->GetTargetTexture());

		for (int i = 0; i < 6; i++)
		{		
			mIrradianceCapture->Bind(i);

			RenderCommand::Clear();
			RenderCommand::SetViewport(0, 0, 32, 32);

			Renderer::Begin();

			Renderer::SubmitCamera(mCubeCamera.GetProjection(), mCubeCamera.GetView({}, i));

			mIrradianceMat->Submit();
			
			auto &submesh = mCube->GetSubMeshes()[0];
			RenderCommand::DrawElementsBaseVertex(ETopologyMode::Triangles, mCube->GetVertexArray(), submesh.StartVertex, submesh.StartIndex, submesh.IndexCount);

			Renderer::End();

			mIrradianceCapture->UnBind();
		}
	}

	void PMREMGenerator::CreatePreFilteredEnvironmentMap()
	{

		mPreFilterEnironmentMat->Submit();
		mPreFilterEnironmentMat->SetTexture("environmentMap", mEnvironmentCapture->GetTargetTexture());

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

			mPreFilterEnironmentMat->SetTexture("imgOutput", mPreFilteredEnvironmentTexture);

			//image.Bind(0, EImageAccess::WRITE, i);

			RenderCommand::Dispath(w / PREFILTER_WORK_GROUP_SIZE, h / PREFILTER_WORK_GROUP_SIZE, 6);
		}

	}

	void PMREMGenerator::CreateBRDFLUTMap()
	{
		mBRDFLUTMat->Submit();
		mBRDFLUTMat->SetTexture("brdfLutTexture", mBRDFLUTTexture);
		RenderCommand::Dispath(BRDF_LUT_SIZE / BRDF_WORK_GROUP_SIZE, BRDF_LUT_SIZE / BRDF_WORK_GROUP_SIZE, 1);
	}
} // namespace BHive
