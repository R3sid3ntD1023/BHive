#include "gfx/Framebuffer.h"
#include "gfx/Image.h"
#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "gfx/Texture.h"
#include "gfx/UniformBuffer.h"
#include "mesh/primitives/Cube.h"
#include "PMREMGenerator.h"
#include "Renderer.h"

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

		mPreFilteredEnvironmentTexture = TextureCube::Create(PREFILTER_MAP_SIZE, pre_filter_specification);

		mBRDFLUTTexture = Texture2D::Create(
			BRDF_LUT_SIZE, BRDF_LUT_SIZE,
			FTextureCreateInfo{.Format = EFormat::RG16F, .WrapMode = EWrapMode::CLAMP_TO_EDGE, .MinFilter = EMinFilter::NEAREST, .MagFilter = EMagFilter::NEAREST});

		mCube = CreateRef<PCube>(2.0f);

		mEquirectangularShader = ShaderManager::Get().Load("Equirectangular.glsl");
		mIrradianceShader = ShaderManager::Get().Load("Irradiance.glsl");
		mPreFilterEnironmentShader = ShaderManager::Get().Load("PrefilterEnvironment.glsl");
		mBRDFLUTShader = ShaderManager::Get().Load("BRDFLut.glsl");

		mInitialized = true;
	}

	void PMREMGenerator::SetEnvironmentMap(const Ref<Texture> &texture)
	{
		mEnvironmentTexture = texture;
		CreateEnvironmentCubeMap();
		CreateIrradianceMap();
		CreateBRDFLUTMap();
		CreatePreFilteredEnvironmentMap();
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
		for (int i = 0; i < 6; i++)
		{
			mEnvironmentCapture->Bind(i);

			RenderCommand::CullFront();
			RenderCommand::Clear();

			//mEquirectangularShader->Bind();
			mEnvironmentTexture->Bind(0);

			RenderCube(i);

			//mEquirectangularShader->UnBind();
			mEnvironmentCapture->UnBind();

			RenderCommand::CullBack();
		}
	}

	void PMREMGenerator::CreateIrradianceMap()
	{

		for (int i = 0; i < 6; i++)
		{
			mIrradianceCapture->Bind(i);

			RenderCommand::CullFront();
			RenderCommand::Clear();

			//mIrradianceShader->Bind();
			mEnvironmentCapture->GetTargetTexture()->Bind();

			RenderCube(i);

			//mIrradianceShader->UnBind();
			mIrradianceCapture->UnBind();

			RenderCommand::CullBack();
		}
	}

	void PMREMGenerator::CreatePreFilteredEnvironmentMap()
	{
		Image image(mPreFilteredEnvironmentTexture);

		//mPreFilterEnironmentShader->Bind();
		mEnvironmentCapture->GetTargetTexture()->Bind();

		int mip_level = (ENVIRONMENT_MAP_SIZE / PREFILTER_MAP_SIZE) - 1;
		for (int i = 0; i < PREFILTER_MIP_LEVELS; i++)
		{
			unsigned w = PREFILTER_MAP_SIZE * pow(0.5f, i);
			unsigned h = PREFILTER_MAP_SIZE * pow(0.5f, i);

			float roughness = (float)i / (float)(PREFILTER_MIP_LEVELS - 1);

			/*mPreFilterEnironmentShader->SetUniform("constants.u_roughness", roughness);
			mPreFilterEnironmentShader->SetUniform("constants.u_mip_level", mip_level);
			mPreFilterEnironmentShader->SetUniform("constants.u_width", w);
			mPreFilterEnironmentShader->SetUniform("constants.u_height", h);*/

			image.Bind(0, EImageAccess::WRITE, i);

			//mPreFilterEnironmentShader->Dispatch(w / PREFILTER_WORK_GROUP_SIZE, h / PREFILTER_WORK_GROUP_SIZE, 6);
		}

		//mPreFilterEnironmentShader->UnBind();
	}

	void PMREMGenerator::CreateBRDFLUTMap()
	{
		/*Image image(mBRDFLUTTexture);

		mBRDFLUTShader->Bind();

		image.Bind(0, EImageAccess::WRITE);
		mBRDFLUTShader->Dispatch(BRDF_LUT_SIZE / BRDF_WORK_GROUP_SIZE, BRDF_LUT_SIZE / BRDF_WORK_GROUP_SIZE);

		mBRDFLUTShader->UnBind();*/
	}

	void PMREMGenerator::RenderCube(uint32_t face)
	{
		Renderer::Begin();
		Renderer::SubmitCamera(mCubeCamera.GetProjection(), mCubeCamera.GetView({}, face));

		auto &submesh = mCube->GetSubMeshes()[0];
		RenderCommand::DrawElementsBaseVertex(ETopologyMode::Triangles, *mCube->GetVertexArray(), submesh.StartVertex, submesh.StartIndex, submesh.IndexCount);

		Renderer::End();
	}

} // namespace BHive
