#include "SolarSystemLayer.h"
#include <gfx/Shader.h>
#include <gfx/UniformBuffer.h>
#include <gfx/RenderCommand.h>
#include "shader.embedded"
#include "Planet.h"

namespace SolarSystem
{
	static std::vector<PlanetData> sPlanetData = {
		PlanetData{"Mercury", BHive::FTransform({4, 0, 0}), RESOURCE_PATH "/Textures/Mercury.jpg"},
		PlanetData{"Venus", BHive::FTransform({8, 0, 0}, {}, {1.9f, 1.9f, 1.9f}), RESOURCE_PATH "/Textures/Venus.jpg"},
		PlanetData{"Earth", BHive::FTransform({12, 0, 0}, {}, {2, 2, 2}), RESOURCE_PATH "/Textures/Earth.jpg"},
		PlanetData{"Mars", BHive::FTransform({18, 0, 0}, {}, {1.3f, 1.3f, 1.3f}), RESOURCE_PATH "/Textures/Mars.jpg"},
		PlanetData{"Jupiter", BHive::FTransform({35, 0, 0}, {}, {10, 10, 10}), RESOURCE_PATH "/Textures/Jupiter.jpg"},
	};

	void SolarSystemLayer::OnAttach()
	{
		mShader = BHive::Shader::Create("plane", s_vertex, s_fragment);

		mUniformBuffers["CameraBuffer"] = BHive::UniformBuffer::Create(0, sizeof(glm::mat4));
		mUniformBuffers["ModelBuffer"] = BHive::UniformBuffer::Create(1, sizeof(glm::mat4));

		auto &window = BHive::Application::Get().GetWindow();
		auto w = window.GetWidth();
		auto h = window.GetHeight();
		mCamera = BHive::EditorCamera(45.f, w / (float)h, .01f, 1000.f);

		for (auto &data : sPlanetData)
		{
			mPlanets.push_back(CreateRef<Planet>(data));
		}
	}
	void SolarSystemLayer::OnUpdate(float dt)
	{
		mModelTransform.add_rotation({0, 10.f * dt, 0});

		mCamera.ProcessInput();

		BHive::RenderCommand::Clear();
		BHive::RenderCommand::ClearColor(.2f, .2f, .2f, 1.f);

		const glm::mat4 view_projection = mCamera.GetProjection() * mCamera.GetView().inverse().to_mat4();
		mUniformBuffers["CameraBuffer"]->SetData(view_projection);

		mUniformBuffers["ModelBuffer"]->SetData(mModelTransform.to_mat4());

		mShader->Bind();

		for (auto &p : mPlanets)
		{
			mUniformBuffers["ModelBuffer"]->SetData(p->GetTransform().to_mat4());
			p->Update(dt);
		}
	}

	void SolarSystemLayer::OnEvent(BHive::Event &e)
	{
		BHive::EventDispatcher dispatcher(e);
		dispatcher.Dispatch(this, &SolarSystemLayer::OnWindowResize);

		mCamera.OnEvent(e);
	}

	bool SolarSystemLayer::OnWindowResize(BHive::WindowResizeEvent &e)
	{
		BHive::RenderCommand::SetViewport(0, 0, e.x, e.y);
		mCamera = BHive::EditorCamera(45.f, e.x / (float)e.y, .01f, 1000.f);
		return false;
	}
} // namespace SolarSystem