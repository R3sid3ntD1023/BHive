#include "CelestrialBody.h"
#include "core/profiler/ProfilerViewer.h"
#include "imgui/ImProfiler.h"
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
#include <mesh/primitives/Plane.h>
#include <renderers/postprocessing/Bloom.h>
#include <renderers/Renderer.h>

#include <audio/AudioImporter.h>
#include <audio/AudioSource.h>
#include <gui/Gui.h>

#include "components/CameraComponent.h"
#include "components/IDComponent.h"
#include "components/TagComponent.h"

#include <font/Font.h>
#include <font/FontManager.h>

BHive::UUID mSelectedID = 1;
BHive::FTextStyle sTextStyle{.TextColor = 0xFF00FFFF};
BHive::FCircleParams sCircleParams;
glm::vec3 sQuadPos = {680, 20, 0};

void SolarSystemLayer::OnAttach()
{
	auto font = BHive::FontManager::Get().AddFontFromFile(ENGINE_PATH "/data/fonts/Roboto/Roboto-Regular.ttf", 96);

	mAudio = BHive::AudioImporter::Import(RESOURCE_PATH
										  "Audio/Resident Evil 5 - 'Rust in Summer 2008' (Versus Mode - Slayers).wav");

	mResourceManager = CreateRef<BHive::ResourceManager>(RESOURCE_PATH);
	BHive::AssetManager::SetAssetManager(&*mResourceManager);

	mUniverse = CreateRef<Universe>();

	{
		std::ifstream in(RESOURCE_PATH "/Data/Universe.json");
		cereal::JSONInputArchive ar(in);
		ar(MAKE_NVP("Universe", *mUniverse));
	}

	mLightingShader = BHive::ShaderLibrary::Load(RESOURCE_PATH "/Shaders/Lighting.glsl");
	mQuadShader = BHive::ShaderLibrary::Load(RESOURCE_PATH "/Shaders/ScreenQuad.glsl");
	mScreenQuad = CreateRef<BHive::PPlane>(1.f, 1.f);

	InitFramebuffer();

	auto &window = BHive::Application::Get().GetWindow();
	auto window_size = window.GetSize();

	mCamera = BHive::EditorCamera(45.f, window_size.x / (float)window_size.y, .01f, 1000.00f);

	BHive::RenderCommand::SetCullEnabled(false);

	BHive::FBloomSettings settings{};
	settings.mFilterThreshold = {1, .95f, .79f, 60.f};
	mBloom = CreateRef<BHive::Bloom>(3, window_size.x / 2, window_size.y / 2, settings);

	BHive::RenderCommand::ClearColor(.2f, .2f, .2f, 1.f);

	mPlayer = mUniverse->AddBody<CelestrialBody>();
	mPlayer->AddComponent<CameraComponent>();
	mPlayer->SetName("Camera");
	mPlayer->SetParent(1);

	// mAudio->Play();
	mUniverse->Begin();

	BHive::RenderCommand::SetLineWidth(1.0f);

	// glEnable(GL_SCISSOR_TEST);
}

void SolarSystemLayer::OnDetach()
{
}

void SolarSystemLayer::OnUpdate(float dt)
{
	mCamera.ProcessInput();

	mMultiSampleFramebuffer->Bind();

	BHive::RenderCommand::Clear();

	auto clear = glm::vec3(0);
	mMultiSampleFramebuffer->ClearAttachment(3, GL_FLOAT, &clear);

	// render scene
	{
		BHive::GetRenderPipelineManager().SetCurrentPipeline(&mPipeline);

		mPipeline.Begin(mCamera.GetProjection(), mCamera.GetView().inverse());

		mUniverse->Update(dt);

		mPipeline.End();
	}

	mMultiSampleFramebuffer->UnBind();
	mMultiSampleFramebuffer->Blit(mFramebuffer);

	BHive::RenderCommand::DisableDepth();

	mLightingbuffer->Bind();
	BHive::RenderCommand::Clear(BHive::Buffer_Color);

	mLightingShader->Bind();

	mFramebuffer->GetColorAttachment()->Bind();
	mFramebuffer->GetColorAttachment(1)->Bind(1);
	mFramebuffer->GetColorAttachment(2)->Bind(2);
	mFramebuffer->GetColorAttachment(3)->Bind(3);

	BHive::RenderCommand::DrawElements(BHive::EDrawMode::Triangles, *mScreenQuad->GetVertexArray());
	mLightingbuffer->UnBind();

	auto &window = BHive::Application::Get().GetWindow();
	auto &size = window.GetSize();

	// glScissor(0, 0, size.x * .5f, size.y);
	BHive::RenderCommand::Clear(BHive::Buffer_Color);

	auto postprocess_texture = mBloom->Process(mFramebuffer->GetColorAttachment(3));

	mQuadShader->Bind();

	auto hdr = mLightingbuffer->GetColorAttachment();
	auto processed_texture = postprocess_texture;

	hdr->Bind();
	processed_texture->Bind(1);
	{
		GPU_PROFILER_SCOPED("LightingQuad");

		BHive::RenderCommand::DrawElements(BHive::EDrawMode::Triangles, *mScreenQuad->GetVertexArray());
	}

	mQuadShader->UnBind();
	// ui

	BHive::Renderer::Begin(glm::ortho(0.f, 800.f, 0.f, 600.f), glm::inverse(glm::mat4(1.f)));
	BHive::QuadRenderer::DrawText(
		36, "Text2D \nNewline Text! \n\ttygp\nfifojoissosogsg\nafianaiofno\naffaffaryy", {.Style = sTextStyle},
		{{50, 200, 0}});
	BHive::Renderer::End();

	BHive::RenderCommand::EnableDepth();

	// mCullingBuffer->Bind();

	// BHive::RenderCommand::Clear();

	// const glm::mat4 t = glm::translate(glm::vec3{35, 0, 200});

	// BHive::GetRenderPipelineManager().SetCurrentPipeline(&mCullingPipeline);
	// mCullingPipeline.Begin(mCamera.GetProjection(), glm::inverse(t));
	// mCullingPipeline.SetTestFrustum(BHive::Frustum(mCamera.GetProjection(), mCamera.GetView().inverse()));

	// BHive::FrustumViewer frustum(mCamera.GetProjection(), mCamera.GetView().inverse());
	// BHive::LineRenderer::DrawFrustum(frustum, 0xFF00FFFF);

	// mUniverse->Update(dt);

	// mCullingPipeline.End();

	// mCullingBuffer->UnBind();
}

void SolarSystemLayer::OnEvent(BHive::Event &e)
{
	BHive::EventDispatcher dispatcher(e);
	dispatcher.Dispatch(this, &SolarSystemLayer::OnWindowResize);
	mCamera.OnEvent(e);
}

void SolarSystemLayer::OnGuiRender()
{
	if (ImGui::Begin("Performance"))
	{

		ImGui::TextColored({1, .5, 0, 1}, "%u", BHive::Renderer::GetStats().DrawCalls);

		ImGui::PushID("Quad");
		ImGui::DragFloat3("Pos", &sQuadPos.x, 0.001f);
		ImGui::PopID();

		ImGui::PushID("TextStyle");
		ImGui::ColorPicker4("Color", &sTextStyle.TextColor.r);
		ImGui::DragFloat("Thickness", &sTextStyle.Thickness, 0.001f, 0, 1);
		ImGui::DragFloat("Smoothness", &sTextStyle.Smoothness, 0.001f, 0, 1);

		ImGui::SeparatorText("Outline");
		ImGui::ColorPicker4("Outline Color", &sTextStyle.OutlineColor.r);
		ImGui::DragFloat("Outline Thickness", &sTextStyle.OutlineThickness, 0.001f, 0, 1);
		ImGui::DragFloat("Outline Smoothness", &sTextStyle.OutlineSmoothness, 0.001f, 0, 1);

		ImGui::PopID();

		ImGui::PushID("Circle");
		ImGui::ColorPicker4("Color", &sCircleParams.LineColor.r);
		ImGui::DragFloat("Thickness", &sCircleParams.Thickness, 0.001f);
		ImGui::DragFloat("Fade", &sCircleParams.Fade, .001f);
		ImGui::DragFloat("Radius", &sCircleParams.Radius, .001f);

		ImGui::PopID();

		BHive::ProfilerViewer::ViewFPS();
		BHive::ProfilerViewer::ViewCPUGPU();
	}

	ImGui::End();

	if (ImGui::Begin("Depth"))
	{
		auto size = ImGui::GetContentRegionAvail();
		auto texture = mFramebuffer->GetDepthAttachment();
		ImGui::Image((ImTextureID)(uint64_t)*texture, size, {0, 1}, {1, 0});
	}
	ImGui::End();
}

void SolarSystemLayer::InitFramebuffer()
{
	auto &window = BHive::Application::Get().GetWindow();
	auto window_size = window.GetSize();

	BHive::FramebufferSpecification specs{};
	specs.Width = window_size.x;
	specs.Height = window_size.y;
	specs.Samples = 4;
	specs.Attachments.attach({.mFormat = BHive::EFormat::RGBA32F, .mWrapMode = BHive::EWrapMode::CLAMP_TO_EDGE})
		.attach({.mFormat = BHive::EFormat::RGB16F, .mWrapMode = BHive::EWrapMode::CLAMP_TO_EDGE})
		.attach({.mFormat = BHive::EFormat::RGB16F, .mWrapMode = BHive::EWrapMode::CLAMP_TO_EDGE})
		.attach({.mFormat = BHive::EFormat::RGB32F, .mWrapMode = BHive::EWrapMode::CLAMP_TO_EDGE})
		.attach({.mFormat = BHive::EFormat::DEPTH24_STENCIL8, .mWrapMode = BHive::EWrapMode::CLAMP_TO_EDGE});

	mMultiSampleFramebuffer = BHive::Framebuffer::Create(specs);

	specs.Samples = 1;
	mFramebuffer = BHive::Framebuffer::Create(specs);

	specs.Attachments.reset().attach({.mFormat = BHive::EFormat::RGBA8, .mWrapMode = BHive::EWrapMode::CLAMP_TO_EDGE});
	mLightingbuffer = BHive::Framebuffer::Create(specs);
}

bool SolarSystemLayer::OnWindowResize(BHive::WindowResizeEvent &e)
{
	mCamera.Resize(e.x, e.y);
	mBloom->Resize(e.x / 2, e.y / 2);
	mMultiSampleFramebuffer->Resize(e.x, e.y);
	mFramebuffer->Resize(e.x, e.y);
	mLightingbuffer->Resize(e.x, e.y);

	return false;
}
