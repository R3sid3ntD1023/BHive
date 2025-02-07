#pragma once

#include <cameras/EditorCamera.h>
#include <core/Application.h>
#include <core/Core.h>
#include <core/Layer.h>

namespace BHive
{
	class UniformBuffer;
	class Shader;
	class Framebuffer;
	class PPlane;

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
		void InitFramebuffer();

	private:
		Ref<BHive::Shader> mShader;
		Ref<BHive::Shader> mQuadShader;

		std::unordered_map<std::string, Ref<BHive::UniformBuffer>> mUniformBuffers;
		BHive::EditorCamera mCamera;

		std::vector<Ref<Planet>> mPlanets;

		Ref<BHive::Framebuffer> mMultiSampleFrameBuffer;
		Ref<BHive::PPlane> mScreenQuad;

		glm::ivec2 mViewportSize{};
	};
} // namespace SolarSystem