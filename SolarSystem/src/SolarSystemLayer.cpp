#include "Planet.h"
#include "SolarSystemLayer.h"
#include <core/serialization/Serialization.h>
#include <gfx/Framebuffer.h>
#include <gfx/RenderCommand.h>
#include <gfx/Shader.h>
#include <gfx/UniformBuffer.h>
#include <mesh/primitives/Plane.h>

namespace SolarSystem
{
	static std::vector<PlanetData> sPlanetData;

	void SolarSystemLayer::OnAttach()
	{
		{
			std::ifstream in(RESOURCE_PATH "/PlanetData.json");
			cereal::JSONInputArchive ar(in);
			ar(sPlanetData);
		}

		mShader = BHive::ShaderLibrary::Load(RESOURCE_PATH "/Shaders/Planet.glsl");
		mQuadShader = BHive::ShaderLibrary::Load(RESOURCE_PATH "/Shaders/ScreenQuad.glsl");
		mScreenQuad = CreateRef<BHive::PPlane>(1.f, 1.f);

		mUniformBuffers["CameraBuffer"] = BHive::UniformBuffer::Create(0, sizeof(glm::mat4));
		mUniformBuffers["ModelBuffer"] = BHive::UniformBuffer::Create(1, sizeof(glm::mat4));

		InitFramebuffer();

		auto &window = BHive::Application::Get().GetWindow();
		auto w = window.GetWidth();
		auto h = window.GetHeight();
		mCamera = BHive::EditorCamera(45.f, w / (float)h, .01f, 1000.f);
		mViewportSize = {w, h};

		for (auto &data : sPlanetData)
		{
			mPlanets.push_back(CreateRef<Planet>(data));
		}

		BHive::RenderCommand::SetCullEnabled(false);
	}

	void SolarSystemLayer::OnUpdate(float dt)
	{
		mCamera.ProcessInput();

		mMultiSampleFrameBuffer->Bind();

		BHive::RenderCommand::ClearColor(.2f, .2f, .2f, 1.f);
		BHive::RenderCommand::Clear();

		const glm::mat4 view_projection = mCamera.GetProjection() * mCamera.GetView().inverse().to_mat4();
		mUniformBuffers["CameraBuffer"]->SetData(view_projection);

		mShader->Bind();

		for (auto &p : mPlanets)
		{
			mUniformBuffers["ModelBuffer"]->SetData(p->GetTransform().to_mat4());
			p->Update(dt);
		}

		mMultiSampleFrameBuffer->UnBind();
		mMultiSampleFrameBuffer->Blit(mFramebuffer);

		BHive::RenderCommand::Clear();

		mQuadShader->Bind();
		// mQuadShader->SetUniform("uViewportSize", mViewportSize);

		// mMultiSampleFrameBuffer->GetColorAttachment()->Bind();
		mFramebuffer->GetColorAttachment()->Bind();
		mFramebuffer->GetColorAttachment(1)->Bind(1);
		mFramebuffer->GetColorAttachment(2)->Bind(2);

		BHive::RenderCommand::DrawElements(BHive::EDrawMode::Triangles, *mScreenQuad->GetVertexArray());
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
			auto scene = mFramebuffer->GetColorAttachment();
			auto pos = mFramebuffer->GetColorAttachment(1);
			auto norm = mFramebuffer->GetColorAttachment(2);

			ImGui::Image((ImTextureID)(uint64_t)*scene, {200, 200}, {0, 1}, {1, 0});
			ImGui::Image((ImTextureID)(uint64_t)*pos, {200, 200}, {0, 1}, {1, 0});
			ImGui::Image((ImTextureID)(uint64_t)*norm, {200, 200}, {0, 1}, {1, 0});
		}

		ImGui::End();
	}

	bool SolarSystemLayer::OnWindowResize(BHive::WindowResizeEvent &e)
	{
		BHive::RenderCommand::SetViewport(0, 0, e.x, e.y);
		mCamera = BHive::EditorCamera(45.f, e.x / (float)e.y, .01f, 1000.f);
		mMultiSampleFrameBuffer->Resize(e.x, e.y);
		mFramebuffer->Resize(e.x, e.y);
		mViewportSize = {e.x, e.y};

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
		specs.Samples = 16;
		specs.Attachments.attach({.mFormat = BHive::EFormat::RGBA32F, .mWrapMode = BHive::EWrapMode::CLAMP_TO_EDGE})
			.attach({.mFormat = BHive::EFormat::RGB16F, .mWrapMode = BHive::EWrapMode::CLAMP_TO_EDGE})
			.attach({.mFormat = BHive::EFormat::RGB16F, .mWrapMode = BHive::EWrapMode::CLAMP_TO_EDGE})
			.attach({.mFormat = BHive::EFormat::DEPTH24_STENCIL8, .mWrapMode = BHive::EWrapMode::CLAMP_TO_EDGE});

		mMultiSampleFrameBuffer = BHive::Framebuffer::Create(specs);
		specs.Samples = 1;
		mFramebuffer = BHive::Framebuffer::Create(specs);
	}
} // namespace SolarSystem