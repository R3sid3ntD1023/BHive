#include "SceneRenderer.h"
#include "renderers/Renderer.h"
#include "gfx/Camera.h"
#include "gfx/Framebuffer.h"
#include "gfx/Shader.h"
#include "gfx/ShaderManager.h"
#include "mesh/primitives/Quad.h"
#include "gfx/RenderCommand.h"
#include "math/Transform.h"
#include "renderers/postprocessing/Bloom.h"
#include "renderers/PMREMGenerator.h"
#include "importers/TextureImporter.h"

namespace BHive
{
#define SCENE_RENDERER_HAS_FLAG(flag) ((mFlags & flag) != 0)

	void SceneRenderer::Initialize(uint32_t width, uint32_t height, uint16_t flags)
	{
		mFlags = flags;

		// Initialize the framebuffer or any other resources needed for rendering
		FramebufferSpecification specs;
		specs.Width = width;
		specs.Height = height;
		specs.Attachments.attach({.InternalFormat = EFormat::RGBA8, .WrapMode = EWrapMode::CLAMP_TO_EDGE})
			.attach({.InternalFormat = EFormat::DEPTH24_STENCIL8, .WrapMode = EWrapMode::CLAMP_TO_EDGE});

		mFramebuffer = CreateRef<Framebuffer>(specs);

		if (SCENE_RENDERER_HAS_FLAG(SceneRendererFlags::Bloom))
		{
			// Initialize bloom post-processing effect if enabled
			mBloom = CreateRef<Bloom>(5, width, height, FBloomSettings{});
		}

		if (SCENE_RENDERER_HAS_FLAG(SceneRendererFlags::RenderQuad))
		{
			// Create a final framebuffer for post-processing effects
			specs.Attachments.reset();
			specs.Attachments.attach({.InternalFormat = EFormat::RGBA8, .WrapMode = EWrapMode::CLAMP_TO_EDGE})
				.attach({.InternalFormat = EFormat::DEPTH24_STENCIL8, .WrapMode = EWrapMode::CLAMP_TO_EDGE});
			mFinalFramebuffer = CreateRef<Framebuffer>(specs);

			mQuad = CreateRef<PQuad>();
			mQuadShader = ShaderManager::Get().Load(ENGINE_PATH "/data/shaders/ScreenQuad.glsl");
		}

		Ref<Texture> environment_texture = TextureLoader::Import(ENGINE_PATH "/data/hdr/industrial_sunset_puresky_2k.hdr");
		EnvironmentMapGenerator.Initialize();
		EnvironmentMapGenerator.SetEnvironmentMap(environment_texture);
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
		Ref<Texture> bloom_texture;

		if (SCENE_RENDERER_HAS_FLAG(SceneRendererFlags::Bloom))
		{
			// Apply post-processing effects if enabled
			bloom_texture = mBloom->Process(texture);
		}

		if (SCENE_RENDERER_HAS_FLAG(SceneRendererFlags::RenderQuad))
		{
			mFinalFramebuffer->Bind();

			RenderCommand::Clear();

			mQuadShader->Bind();

			float bloom_strength = 1.0f;
			uint32_t flags = BIT(0);

			mQuadShader->SetUniform("constants.u_BloomStrength", bloom_strength);
			mQuadShader->SetUniform("constants.u_PostProcessMode", flags);

			texture->Bind(0);
			bloom_texture->Bind(1);

			RenderCommand::DrawElements(EDrawMode::Triangles, *mQuad->GetVertexArray());

			mFinalFramebuffer->UnBind();
		}
	}

	void SceneRenderer::SetEnvironmentMap(const Ref<Texture> &environment)
	{
		EnvironmentMapGenerator.SetEnvironmentMap(environment);
	}

	void SceneRenderer::AddPostProcessingEffect(const Ref<PostProcessor> &processor)
	{
		mPostProcessingEffects.push_back(processor);
	}

	void SceneRenderer::Resize(uint32_t width, uint32_t height)
	{
		mFramebuffer->Resize(width, height);

		if (SCENE_RENDERER_HAS_FLAG(SceneRendererFlags::Bloom))
			mBloom->Resize(width, height);

		if (SCENE_RENDERER_HAS_FLAG(SceneRendererFlags::RenderQuad))
			mFinalFramebuffer->Resize(width, height);
	}

	const Ref<Texture> &SceneRenderer::GetColorAttachment(uint32_t index) const
	{
		return SCENE_RENDERER_HAS_FLAG(SceneRendererFlags::RenderQuad) ? mFinalFramebuffer->GetColorAttachment(index)
																	   : mFramebuffer->GetColorAttachment(index);
	}
} // namespace BHive