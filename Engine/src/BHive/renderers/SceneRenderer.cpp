#include "SceneRenderer.h"
#include "renderers/Renderer.h"
#include "gfx/Camera.h"
#include "gfx/Framebuffer.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "mesh/primitives/Quad.h"
#include "gfx/RenderCommand.h"
#include "core/math/Transform.h"
#include "renderers/postprocessing/Bloom.h"
#include "renderers/PMREMGenerator.h"
#include "importers/TextureImporter.h"
#include "postprocessing/Aces.h"
#include "gfx/textures/Texture2D.h"

namespace BHive
{

	void SceneRenderer::Initialize(uint32_t width, uint32_t height)
	{

		// Initialize the framebuffer or any other resources needed for rendering
		FramebufferSpecification specs;
		specs.Width = width;
		specs.Height = height;
		specs.Attachments.attach({.InternalFormat = EFormat::RGBA8, .WrapMode = EWrapMode::CLAMP_TO_EDGE})
			.attach({.InternalFormat = EFormat::DEPTH24_STENCIL8, .WrapMode = EWrapMode::CLAMP_TO_EDGE});

		mFramebuffer = CreateRef<Framebuffer>(specs);

		// Initialize bloom post-processing effect if enabled
		AddPostProcessingEffect(CreateRef<Bloom>(5, width, height, FBloomSettings{}));
		AddPostProcessingEffect(CreateRef<Aces>(width, height));

		// Create a final framebuffer for post-processing effects
		specs.Attachments.reset();
		specs.Attachments.attach({.InternalFormat = EFormat::RGBA8, .WrapMode = EWrapMode::CLAMP_TO_EDGE})
			.attach({.InternalFormat = EFormat::DEPTH24_STENCIL8, .WrapMode = EWrapMode::CLAMP_TO_EDGE});
		mFinalFramebuffer = CreateRef<Framebuffer>(specs);

		// Create a quad for rendering the final output
		mQuad = CreateRef<PQuad>();
		mQuadShader = ShaderManager::Get().Load(ENGINE_PATH "/data/shaders/ScreenQuad.glsl");

		// Initialize the PMREM generator
		EnvironmentMapGenerator.Initialize();

		// Load the environment map if it is not already set
		if (!sEnvironmentMap)
		{
			sEnvironmentMap = TextureLoader::Import(ENGINE_PATH "/data/hdr/industrial_sunset_puresky_2k.hdr");
			EnvironmentMapGenerator.SetEnvironmentMap(sEnvironmentMap);
		}

		mRenderSize = {width, height};
	}

	void SceneRenderer::Begin(const Camera *camera, const FTransform &view)
	{
		mFramebuffer->Bind();

		RenderCommand::Clear();

		Renderer::Begin();
		Renderer::SubmitCamera(camera->GetProjection(), view.inverse());
	}

	void SceneRenderer::End()
	{
		EnvironmentMapGenerator.GetPreFilteredEnvironmentTetxure()->Bind(0);
		EnvironmentMapGenerator.GetIrradianceTexture()->Bind(1);
		EnvironmentMapGenerator.GetBDRFLUT()->Bind(2);

		Renderer::End();

		mFramebuffer->UnBind();

		auto texture = mFramebuffer->GetColorAttachment(0);

		for (const auto &effect : mPostProcessingEffects)
		{
			texture = effect->Process(texture);
		}

		mFinalFramebuffer->Bind();

		RenderCommand::Clear();

		mQuadShader->Bind();

		texture->Bind();

		RenderCommand::DrawElements(EDrawMode::Triangles, *mQuad->GetVertexArray());

		mFinalFramebuffer->UnBind();
	}

	void SceneRenderer::SetEnvironmentMap(const Ref<Texture2D> &environment)
	{
		sEnvironmentMap = environment;
		EnvironmentMapGenerator.SetEnvironmentMap(environment);
	}

	void SceneRenderer::AddPostProcessingEffect(const Ref<PostProcessor> &processor)
	{
		mPostProcessingEffects.push_back(processor);
	}

	void SceneRenderer::Resize(uint32_t width, uint32_t height)
	{
		mFramebuffer->Resize(width, height);

		mFinalFramebuffer->Resize(width, height);

		for (auto &effect : mPostProcessingEffects)
		{
			effect->Resize(width, height);
		}

		mRenderSize = {width, height};
	}

	const Ref<Texture> &SceneRenderer::GetColorAttachment(uint32_t index) const
	{
		return mFinalFramebuffer->GetColorAttachment(index);
	}

	const Ref<Texture2D> &SceneRenderer::GetEnvironmentMap() const
	{
		return sEnvironmentMap;
	}

	glm::uvec2 SceneRenderer::GetSize() const
	{
		return mRenderSize;
	}

	REFLECT(FRenderSettings)
	{
		BEGIN_REFLECT(FRenderSettings);
	}

	REFLECT(SceneRenderer)
	{
		BEGIN_REFLECT(SceneRenderer)
		REFLECT_PROPERTY("PostProcessEffects", mPostProcessingEffects)
		REFLECT_PROPERTY("EnvironmentMap", GetEnvironmentMap, SetEnvironmentMap);
	}
} // namespace BHive