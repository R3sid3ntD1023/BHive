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

#include "ComponentSystems/CullingSystem.h"

void SolarSystemLayer::OnAttach()
{
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
	auto w = window.GetWidth();
	auto h = window.GetHeight();
	mCamera = BHive::EditorCamera(45.f, w / (float)h, .01f, 1000.f);
	mViewportSize = {w, h};

	BHive::RenderCommand::SetCullEnabled(false);

	BHive::FBloomSettings settings{};
	settings.mFilterThreshold = {1, .95f, .79f, 60.f};
	mBloom = CreateRef<BHive::Bloom>(5, w, h, settings);

	BHive::RenderCommand::ClearColor(.2f, .2f, .2f, 1.f);
}

void SolarSystemLayer::OnDetach()
{
}

void SolarSystemLayer::OnUpdate(float dt)
{
	CPU_PROFILER_FUNCTION();
	GPU_PROFILER_SCOPED("GPU- SolarSystem::Update");

	mCounter.Frame();

	mCamera.ProcessInput();

	mMultiSampleFramebuffer->Bind();

	BHive::RenderCommand::Clear();

	auto clear = glm::vec3(0);
	mMultiSampleFramebuffer->ClearAttachment(3, GL_FLOAT, &clear);

	BHive::Renderer::Begin(mCamera.GetProjection(), mCamera.GetView().inverse());

	// BHive::Frustum frustum(mCamera.GetProjection(), mCamera.GetView().inverse());
	// BHive::LineRenderer::DrawFrustum(frustum, 0xFF00FFFF);

	mUniverse->Update(dt);

	BHive::Renderer::End();

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

	{
		GPU_PROFILER_SCOPED("DrawGBufferQuad");
		BHive::RenderCommand::DrawElements(BHive::EDrawMode::Triangles, *mScreenQuad->GetVertexArray());
	}

	mLightingbuffer->UnBind();

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

	BHive::RenderCommand::EnableDepth();

	mCullingBuffer->Bind();

	BHive::RenderCommand::Clear();
	BHive::Renderer::Begin(mCamera.GetProjection(), glm::inverse(glm::translate(glm::vec3{8, 20, 0})));

	mCullingSystem->Update(mUniverse.get(), dt);

	BHive::Renderer::End();

	mCullingBuffer->UnBind();

	mFPS[mCurrentIndex++] = (float)(mCounter);

	mCurrentIndex = mCurrentIndex % mFPS.size();
}

void SolarSystemLayer::OnEvent(BHive::Event &e)
{
	BHive::EventDispatcher dispatcher(e);
	dispatcher.Dispatch(this, &SolarSystemLayer::OnWindowResize);

	mCamera.OnEvent(e);
}

void SolarSystemLayer::OnGuiRender(float)
{
	if (ImGui::Begin("GBuffer"))
	{
		if (ImGui::BeginTable("Settings", 2))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();

			auto scene = mFramebuffer->GetColorAttachment();
			auto pos = mFramebuffer->GetColorAttachment(1);
			auto norm = mFramebuffer->GetColorAttachment(2);
			auto emission = mFramebuffer->GetColorAttachment(3);

			ImGui::Image((ImTextureID)(uint64_t)*scene, {200, 200}, {0, 1}, {1, 0});
			ImGui::Image((ImTextureID)(uint64_t)*pos, {200, 200}, {0, 1}, {1, 0});
			ImGui::Image((ImTextureID)(uint64_t)*norm, {200, 200}, {0, 1}, {1, 0});
			ImGui::Image((ImTextureID)(uint64_t)*emission, {200, 200}, {0, 1}, {1, 0});

			ImGui::TableNextColumn();

			auto &bloom = mBloom->GetSettings();
			ImGui::DragFloat("FilterRadius", &bloom.mFilterRadius, .01f, 0.0f, 1.0f);
			ImGui::ColorEdit4("FilterThreshold", &bloom.mFilterThreshold.x, ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);
		}

		ImGui::EndTable();
	}

	ImGui::End();

	if (ImGui::Begin("Importer"))
	{
		if (ImGui::Button("Import"))
		{
			const char *filter =
				"All (*.*)\0*.*\0 JPG (*.jpg)\0*.jpg\0 PNG (*.png)\0*.png\0 GLB (*.glb)\0*.glb\0 GLTF (*.gltf)\0*.gltf\0 OBJ (*.obj)\0*.obj\0";
			auto str = BHive::FileDialogs::OpenFile(filter);
			if (!str.empty())
			{
				mResourceManager->Import(str);
			}
		}
	}

	ImGui::End();

	if (ImGui::Begin("Performance"))
	{
		BHive::ProfilerViewer::ViewFPS(mCounter, mFPS.data(), mFPS.size());
		BHive::ProfilerViewer::ViewCPUGPU();
	}

	ImGui::End();

	if (ImGui::Begin("Culling"))
	{
		auto size = ImGui::GetContentRegionAvail();
		ImGui::Image((ImTextureID)(uint64_t)*mCullingBuffer->GetColorAttachment(), size, {0, 1}, {1, 0});
	}

	ImGui::End();
}

bool SolarSystemLayer::OnWindowResize(BHive::WindowResizeEvent &e)
{
	mCamera.Resize(e.x, e.y);
	mMultiSampleFramebuffer->Resize(e.x, e.y);
	mFramebuffer->Resize(e.x, e.y);
	mLightingbuffer->Resize(e.x, e.y);
	mViewportSize = {e.x, e.y};
	mBloom->Resize(e.x, e.y);

	mCullingBuffer->Resize(e.x, e.y);

	return false;
}

void SolarSystemLayer::InitFramebuffer()
{
	auto &window = BHive::Application::Get().GetWindow();
	auto w = window.GetWidth();
	auto h = window.GetHeight();

	BHive::FramebufferSpecification specs{};
	specs.Width = w;
	specs.Height = h;
	specs.Samples = 32;
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

	specs.Attachments.reset()
		.attach({.mFormat = BHive::EFormat::RGBA8, .mWrapMode = BHive::EWrapMode::CLAMP_TO_EDGE})
		.attach({.mFormat = BHive::EFormat::DEPTH24_STENCIL8, .mWrapMode = BHive::EWrapMode::CLAMP_TO_EDGE});
	mCullingBuffer = BHive::Framebuffer::Create(specs);

	mCullingSystem = CreateRef<CullingSystem>();
}
