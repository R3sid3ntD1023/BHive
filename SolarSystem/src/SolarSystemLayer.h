#pragma once

#include <core/Core.h>
#include <core/Layer.h>
#include <cameras/EditorCamera.h>
#include <core/Application.h>

namespace BHive
{
	class UniformBuffer;
	class Shader;

} // namespace BHive

namespace SolarSystem
{
	struct Planet;

	struct SolarSystemLayer : public BHive::Layer
	{
		virtual void OnAttach() override;

		virtual void OnUpdate(float dt) override;

		virtual void OnEvent(BHive::Event &e);

		bool OnWindowResize(BHive::WindowResizeEvent &e);

	private:
		Ref<BHive::Shader> mShader;
		std::unordered_map<std::string, Ref<BHive::UniformBuffer>> mUniformBuffers;
		BHive::EditorCamera mCamera;

		std::vector<Ref<Planet>> mPlanets;

		BHive::FTransform mModelTransform;
	};
} // namespace SolarSystem