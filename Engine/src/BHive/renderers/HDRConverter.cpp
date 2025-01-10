#include "HDRConverter.h"
#include "gfx/Framebuffer.h"
#include "gfx/Shader.h"
#include "gfx/Texture.h"
#include "mesh/StaticMesh.h"
#include "gfx/RenderCommand.h"
#include <glad/glad.h>

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
    const static glm::mat4 views[] =
        {
            glm::lookAt(glm::vec3{}, {1, 0, 0}, {0, -1, 0}),
            glm::lookAt(glm::vec3{}, {-1, 0, 0}, {0, -1, 0}),
            glm::lookAt(glm::vec3{}, {0, 1, 0}, {0, 0, 1}),
            glm::lookAt(glm::vec3{}, {0, -1, 0}, {0, 0, -1}),
            glm::lookAt(glm::vec3{}, {0, 0, 1}, {0, -1, 0}),
            glm::lookAt(glm::vec3{}, {0, 0, -1}, {0, -1, 0})};

    HDRConverter::HDRConverter()
    {
        FramebufferSpecification envrioment_capture_fbo_specs{};
        envrioment_capture_fbo_specs.Width = 512;
        envrioment_capture_fbo_specs.Height = 512;
		envrioment_capture_fbo_specs.Attachments.attach({.mFormat = EFormat::RGB16F,
														 .mWrapMode = EWrapMode::CLAMP_TO_EDGE,
														 .mMips = true,
														 .mLevels = 5,
														 .mType = ETextureType::TEXTURE_CUBE_MAP

														},
														ETextureType::TEXTURE_CUBE_MAP);
        mEnvironmentCaptureFBO = Framebuffer::Create(envrioment_capture_fbo_specs);

        envrioment_capture_fbo_specs.Width = IRRANDIANCE_CUBEMAP_SIZE;
        envrioment_capture_fbo_specs.Height = IRRANDIANCE_CUBEMAP_SIZE;
		envrioment_capture_fbo_specs.Attachments.reset().attach(
			{
                .mFormat = EFormat::RGBA32F,
			    .mWrapMode = EWrapMode::CLAMP_TO_EDGE,
			     .mType = ETextureType::TEXTURE_CUBE_MAP

			},
			ETextureType::TEXTURE_CUBE_MAP);
		mIrradianceFBO = Framebuffer::Create(envrioment_capture_fbo_specs);

        mPreFilteredEnvironmentTexture =
			TextureCube::Create(PREFILTER_MAP_SIZE, {FTextureSpecification{
														.mFormat = EFormat::RGBA16F,
														.mWrapMode = EWrapMode::CLAMP_TO_EDGE,
														.mMinFilter = EFilterMode::MIPMAP_LINEAR,
														.mMagFilter = EFilterMode::LINEAR,
														.mMips = true,
														.mLevels = PREFILTER_MIP_LEVELS,
														.mType = ETextureType::TEXTURE_CUBE_MAP,
													}});

        mBRDFLUTTexture = Texture2D::Create(BRDF_LUT_SIZE, BRDF_LUT_SIZE,
											FTextureSpecification{
												.mFormat = EFormat::RG16F,
												.mWrapMode = EWrapMode::CLAMP_TO_EDGE,
												.mMinFilter = EFilterMode::NEAREST,
												.mMagFilter = EFilterMode::NEAREST,
											});

        mCube = StaticMesh::CreateCube(1.f);

        mEquirectangularShader = ShaderLibrary::Load(ENGINE_PATH "/data/shaders/Equirectangular.glsl");

        mIrradianceShader = ShaderLibrary::Load(ENGINE_PATH "/data/shaders/Irradiance.glsl");
        mPreFilterEnironmentShader = ShaderLibrary::Load(ENGINE_PATH "/data/shaders/PreFilterEnironmentShader.glsl");
        mBRDFLUTShader = ShaderLibrary::Load(ENGINE_PATH "/data/shaders/BRDFLUT.glsl");
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
        RenderCommand::DrawElementsBaseVertex(EDrawMode::Triangles, *mCube->GetVertexArray(),
                                              submesh.mStartVertex, submesh.mStartIndex, submesh.mIndexCount);

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
        RenderCommand::DrawElementsBaseVertex(EDrawMode::Triangles, *mCube->GetVertexArray(),
                                              submesh.mStartVertex, submesh.mStartIndex, submesh.mIndexCount);

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
}
