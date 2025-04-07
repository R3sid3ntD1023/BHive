#include "CelestrialBody.h"
#include "core/profiler/ProfilerViewer.h"
#include "SolarSystemLayer.h"
#include "Universe.h"
#include <asset/AssetManager.h>
#include <core/FileDialog.h>
#include <core/serialization/Serialization.h>
#include <gfx/Framebuffer.h>
#include <gfx/RenderCommand.h>
#include <gfx/Shader.h>
#include <gfx/UniformBuffer.h>
#include <glad/glad.h>
#include <implot.h>
#include <mesh/primitives/Quad.h>
#include <renderers/postprocessing/Bloom.h>
#include <renderers/Renderer.h>

#include <audio/AudioImporter.h>
#include <audio/AudioSource.h>
#include <gui/Gui.h>

#include "components/CameraComponent.h"

#include "gfx/ShaderManager.h"
#include "utils/ImageUtils.h"
#include <font/Font.h>
#include <font/FontManager.h>

BEGIN_NAMESPACE(BHive)

UUID mSelectedID{"f68f4384-d55f-4a9b-872e-efe27fa06659"};

void SolarSystemLayer::OnAttach()
{
	ShaderManager::Get().LoadFiles(RESOURCE_PATH "Shaders");

	auto font = FontManager::Get().AddFontFromFile(ENGINE_PATH "/data/fonts/Roboto/Roboto-Regular.ttf", 96);

	mAudio =
		AudioImporter::Import(RESOURCE_PATH "Audio/Resident Evil 5 - 'Rust in Summer 2008' (Versus Mode - Slayers).wav");

	mResourceManager = CreateRef<ResourceManager>(RESOURCE_PATH);
	AssetManager::SetAssetManager(&*mResourceManager);

	mUniverse = CreateRef<Universe>();

	{
		std::ifstream in(RESOURCE_PATH "/Data/Universe.json");
		cereal::JSONInputArchive ar(in);
		ar(MAKE_NVP("Universe", *mUniverse));
	}

	mLightingShader = ShaderManager::Get().Get("Lighting.glsl");
	mQuadShader = ShaderManager::Get().Get("ScreenQuad.glsl");
	mScreenQuad = CreateRef<PQuad>();

	InitFramebuffer();

	auto &window = Application::Get().GetWindow();
	auto window_size = window.GetSize();

	mCamera = EditorCamera(45.f, window_size.x / (float)window_size.y, .01f, 1000.00f);

	RenderCommand::SetCullEnabled(false);

	FBloomSettings settings{};
	settings.mFilterThreshold = {1, .95f, .79f, 60.f};
	mBloom = CreateRef<Bloom>(3, window_size.x / 2, window_size.y / 2, settings);

	RenderCommand::ClearColor(.2f, .2f, .2f, 1.f);

	// mAudio->Play();
	mUniverse->Begin();

	RenderCommand::SetLineWidth(1.0f);

	// glEnable(GL_SCISSOR_TEST);
}

void SolarSystemLayer::OnDetach()
{
}

void SolarSystemLayer::OnUpdate(float dt)
{
	mCamera.ProcessInput();

	mMultiSampleFramebuffer->Bind();

	RenderCommand::Clear();

	auto clear = glm::vec3(0);
	mMultiSampleFramebuffer->ClearAttachment(3, GL_FLOAT, &clear.x);

	// render scene
	{
		GetRenderPipelineManager().SetCurrentPipeline(&mPipeline);

		mPipeline.Begin(mCamera.GetProjection(), mCamera.GetView());

		LineRenderer::DrawGrid(FGrid{});

		mUniverse->Update(dt);

		mPipeline.End();
	}

	mMultiSampleFramebuffer->UnBind();
	mMultiSampleFramebuffer->Blit(mFramebuffer);

	RenderCommand::DisableDepth();

	mLightingbuffer->Bind();
	RenderCommand::Clear(Buffer_Color);

	mLightingShader->Bind();

	mFramebuffer->GetColorAttachment()->Bind();
	mFramebuffer->GetColorAttachment(1)->Bind(1);
	mFramebuffer->GetColorAttachment(2)->Bind(2);
	mFramebuffer->GetColorAttachment(3)->Bind(3);

	RenderCommand::DrawElements(EDrawMode::Triangles, *mScreenQuad->GetVertexArray());
	mLightingbuffer->UnBind();

	auto &window = Application::Get().GetWindow();
	auto &size = window.GetSize();

	RenderCommand::Clear(Buffer_Color);

	auto postprocess_texture = mBloom->Process(mFramebuffer->GetColorAttachment(3));

	mQuadShader->Bind();

	auto hdr = mLightingbuffer->GetColorAttachment();
	auto processed_texture = postprocess_texture;

	hdr->Bind();
	processed_texture->Bind(1);
	{
		GPU_PROFILER_SCOPED("LightingQuad");

		RenderCommand::DrawElements(EDrawMode::Triangles, *mScreenQuad->GetVertexArray());
	}

	mQuadShader->UnBind();

	RenderCommand::EnableDepth();
}

void SolarSystemLayer::OnEvent(Event &e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch(this, &SolarSystemLayer::OnWindowResize);
	mCamera.OnEvent(e);
}

void SolarSystemLayer::OnGuiRender()
{
	if (ImGui::Begin("Performance"))
	{
		ProfilerViewer::ViewFPS();
		ProfilerViewer::ViewCPUGPU();
	}

	ImGui::End();

	if (ImGui::Begin("Depth"))
	{
		auto size = ImGui::GetContentRegionAvail();
		auto texture = mFramebuffer->GetDepthAttachment();
		ImGui::Image((ImTextureID)(uint64_t)*texture, size, {0, 1}, {1, 0});
	}
	ImGui::End();

	if (ImGui::Begin("Save"))
	{
		if (ImGui::Button("Save"))
		{
			auto path = FileDialogs::SaveFile("PNG (.png)\0*.png\0");
			if (!path.empty())
			{
				ImageUtils::SaveImage(path, mFramebuffer);
			}
		}
	}

	ImGui::End();
}

void SolarSystemLayer::InitFramebuffer()
{
	auto &window = Application::Get().GetWindow();
	auto window_size = window.GetSize();

	FramebufferSpecification specs{};
	specs.Width = window_size.x;
	specs.Height = window_size.y;
	specs.Samples = 4;
	specs.Attachments.attach({.InternalFormat = EFormat::RGBA32F, .WrapMode = EWrapMode::CLAMP_TO_EDGE})
		.attach({.InternalFormat = EFormat::RGB16F, .WrapMode = EWrapMode::CLAMP_TO_EDGE})
		.attach({.InternalFormat = EFormat::RGB16F, .WrapMode = EWrapMode::CLAMP_TO_EDGE})
		.attach({.InternalFormat = EFormat::RGB32F, .WrapMode = EWrapMode::CLAMP_TO_EDGE})
		.attach({.InternalFormat = EFormat::DEPTH24_STENCIL8, .WrapMode = EWrapMode::CLAMP_TO_EDGE});

	mMultiSampleFramebuffer = CreateRef<Framebuffer>(specs);

	specs.Samples = 1;
	mFramebuffer = CreateRef<Framebuffer>(specs);

	specs.Attachments.reset().attach({.InternalFormat = EFormat::RGBA8, .WrapMode = EWrapMode::CLAMP_TO_EDGE});
	mLightingbuffer = CreateRef<Framebuffer>(specs);
}

bool SolarSystemLayer::OnWindowResize(WindowResizeEvent &e)
{
	mCamera.Resize(e.x, e.y);
	mBloom->Resize(e.x / 2, e.y / 2);
	mMultiSampleFramebuffer->Resize(e.x, e.y);
	mFramebuffer->Resize(e.x, e.y);
	mLightingbuffer->Resize(e.x, e.y);

	return false;
}

END_NAMESPACE
