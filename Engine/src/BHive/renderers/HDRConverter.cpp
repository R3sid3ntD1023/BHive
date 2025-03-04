#include "gfx/Framebuffer.h"
#include "gfx/RenderCommand.h"
#include "gfx/Shader.h"
#include "gfx/textures/TextureCube.h"
#include "gfx/textures/Texture2D.h"
#include "HDRConverter.h"
#include "mesh/primitives/Cube.h"
#include <glad/glad.h>
#include "gfx/ShaderManager.h"

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
	const static glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.f, .1f, 10.f);
	const static glm::mat4 views[] = {
		glm::lookAt(glm::vec3{}, {1, 0, 0}, {0, -1, 0}), glm::lookAt(glm::vec3{}, {-1, 0, 0}, {0, -1, 0}),
		glm::lookAt(glm::vec3{}, {0, 1, 0}, {0, 0, 1}),	 glm::lookAt(glm::vec3{}, {0, -1, 0}, {0, 0, -1}),
		glm::lookAt(glm::vec3{}, {0, 0, 1}, {0, -1, 0}), glm::lookAt(glm::vec3{}, {0, 0, -1}, {0, -1, 0})};

	HDRConverter::HDRConverter()
	{
		FTextureSpecification environment_specification;
		environment_specification.InternalFormat = EFormat::RGB16F;
		environment_specification.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		environment_specification.Levels = 5;

		FramebufferSpecification envrioment_capture_fbo_specs{};
		envrioment_capture_fbo_specs.Width = 512;
		envrioment_capture_fbo_specs.Height = 512;
		envrioment_capture_fbo_specs.Attachments.attach({environment_specification}, ETextureType::TEXTURE_CUBE_MAP);
		mEnvironmentCaptureFBO = CreateRef<Framebuffer>(envrioment_capture_fbo_specs);

		FTextureSpecification irradiance_specification;
		irradiance_specification.InternalFormat = EFormat::RGBA32F;
		irradiance_specification.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		envrioment_capture_fbo_specs.Width = IRRANDIANCE_CUBEMAP_SIZE;
		envrioment_capture_fbo_specs.Height = IRRANDIANCE_CUBEMAP_SIZE;
		envrioment_capture_fbo_specs.Attachments.reset().attach({irradiance_specification}, ETextureType::TEXTURE_CUBE_MAP);
		mIrradianceFBO = CreateRef<Framebuffer>(envrioment_capture_fbo_specs);

		FTextureSpecification pre_filter_specification;
		pre_filter_specification.InternalFormat = EFormat::RGBA16F;
		pre_filter_specification.WrapMode = EWrapMode::CLAMP_TO_EDGE;
		pre_filter_specification.MinFilter = EMinFilter::MIPMAP_LINEAR;
		pre_filter_specification.MagFilter = EMagFilter::LINEAR;
		pre_filter_specification.Levels = PREFILTER_MIP_LEVELS;

		mPreFilteredEnvironmentTexture = CreateRef<TextureCube>(PREFILTER_MAP_SIZE, pre_filter_specification);

		mBRDFLUTTexture = CreateRef<Texture2D>(
			BRDF_LUT_SIZE, BRDF_LUT_SIZE,
			FTextureSpecification{
				.InternalFormat = EFormat::RG16F,
				.WrapMode = EWrapMode::CLAMP_TO_EDGE,
				.MinFilter = EMinFilter::NEAREST,
				.MagFilter = EMagFilter::NEAREST,
			});

		mCube = CreateRef<PCube>(1.0f);

		mEquirectangularShader = ShaderManager::Get().Load(ENGINE_PATH "/data/shaders/Equirectangular.glsl");

		mIrradianceShader = ShaderManager::Get().Load(ENGINE_PATH "/data/shaders/Irradiance.glsl");
		mPreFilterEnironmentShader = ShaderManager::Get().Load(ENGINE_PATH "/data/shaders/PreFilterEnironmentShader.glsl");
		mBRDFLUTShader = ShaderManager::Get().Load(ENGINE_PATH "/data/shaders/BRDFLUT.glsl");
	}

	void HDRConverter::SetEnvironmentMap(const Ref<Texture> &texture)
	{
		mEnvironmentTexture = texture;
		CreateEnvironmentCubeMap();
		CreateIrradianceMap();
		CreateBRDFLUTMap();
		CreatePreFilteredEnvironmentMap();

		mPreFilteredEnvironmentTexture->Bind(0);
		mIrradianceFBO->GetColorAttachment()->Bind(1);
		mBRDFLUTTexture->Bind(2);
	}

	const Ref<Texture> &HDRConverter::GetPreFilteredEnvironmentTetxure() const
	{
		return mPreFilteredEnvironmentTexture;
	}

	const Ref<Texture> &HDRConverter::GetBDRFLUT() const
	{
		return mBRDFLUTTexture;
	}

	void HDRConverter::CreateEnvironmentCubeMap()
	{

		mEnvironmentCaptureFBO->Bind();

		RenderCommand::ClearColor(.1f, .1f, .1f, 1.f);
		RenderCommand::CullFront();
		RenderCommand::Clear();

		mEquirectangularShader->Bind();
		mEquirectangularShader->SetUniform("u_projection", projection);
		mEnvironmentTexture->Bind(0);

		for (unsigned i = 0; i < 6; i++)
		{

			auto name = "u_views[" + std::to_string(i) + "]";
			mEquirectangularShader->SetUniform(name.c_str(), views[i]);
		}

		auto &submesh = mCube->GetSubMeshes()[0];
		RenderCommand::DrawElementsBaseVertex(
			EDrawMode::Triangles, *mCube->GetVertexArray(), submesh.StartVertex, submesh.StartIndex, submesh.IndexCount);

		mEquirectangularShader->UnBind();
		mEnvironmentCaptureFBO->UnBind();

		mEnvironmentCaptureFBO->GetColorAttachment()->GenerateMipMaps();

		RenderCommand::CullBack();
	}

	void HDRConverter::CreateIrradianceMap()
	{

		mIrradianceFBO->Bind();

		RenderCommand::ClearColor(.1f, .1f, .1f, 1.f);
		RenderCommand::CullFront();
		RenderCommand::Clear();

		mIrradianceShader->Bind();
		mIrradianceShader->SetUniform("u_projection", projection);
		mEnvironmentCaptureFBO->GetColorAttachment()->Bind();

		for (unsigned i = 0; i < 6; i++)
		{

			auto name = "u_views[" + std::to_string(i) + "]";
			mIrradianceShader->SetUniform(name.c_str(), views[i]);
		}

		auto &submesh = mCube->GetSubMeshes()[0];
		RenderCommand::DrawElementsBaseVertex(
			EDrawMode::Triangles, *mCube->GetVertexArray(), submesh.StartVertex, submesh.StartIndex, submesh.IndexCount);

		mIrradianceShader->UnBind();
		mIrradianceFBO->UnBind();

		RenderCommand::CullBack();
	}

	void HDRConverter::CreatePreFilteredEnvironmentMap()
	{
		mPreFilterEnironmentShader->Bind();
		mEnvironmentCaptureFBO->GetColorAttachment()->Bind();

		int mip_level = (ENVIRONMENT_MAP_SIZE / PREFILTER_MAP_SIZE) - 1;
		for (int i = 0; i < PREFILTER_MIP_LEVELS; i++)
		{
			unsigned w = PREFILTER_MAP_SIZE * pow(0.5f, i);
			unsigned h = PREFILTER_MAP_SIZE * pow(0.5f, i);

			float roughness = (float)i / (float)(PREFILTER_MIP_LEVELS - 1);
			mPreFilterEnironmentShader->SetUniform("u_roughness", roughness);
			mPreFilterEnironmentShader->SetUniform("u_mip_level", mip_level);
			mPreFilterEnironmentShader->SetUniform("u_width", w);
			mPreFilterEnironmentShader->SetUniform("u_height", h);

			mPreFilteredEnvironmentTexture->BindAsImage(0, GL_WRITE_ONLY, i);

			mPreFilterEnironmentShader->Dispatch(w / PREFILTER_WORK_GROUP_SIZE, h / PREFILTER_WORK_GROUP_SIZE, 6);
		}

		mPreFilterEnironmentShader->UnBind();
	}

	void HDRConverter::CreateBRDFLUTMap()
	{
		mBRDFLUTShader->Bind();

		mBRDFLUTTexture->BindAsImage(0, GL_WRITE_ONLY);
		mBRDFLUTShader->Dispatch(BRDF_LUT_SIZE / BRDF_WORK_GROUP_SIZE, BRDF_LUT_SIZE / BRDF_WORK_GROUP_SIZE);

		mBRDFLUTShader->UnBind();
	}
} // namespace BHive
