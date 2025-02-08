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
	class Bloom;

} // namespace BHive

namespace SolarSystem
{
	struct CelestrialBody;

	struct SolarSystemLayer : public BHive::Layer
	{
		virtual void OnAttach() override;

		virtual void OnUpdate(float dt) override;

		virtual void OnEvent(BHive::Event &e);

		virtual void OnGuiRender(float) override;

		bool OnWindowResize(BHive::WindowResizeEvent &e);

	private:
		void InitFramebuffer();

	private:
		Ref<BHive::Shader> mShader;
		Ref<BHive::Shader> mLightingShader;
		Ref<BHive::Shader> mQuadShader;

		BHive::EditorCamera mCamera;

		std::vector<Ref<CelestrialBody>> mBodies;

		Ref<BHive::Framebuffer> mMultiSampleFrameBuffer;
		Ref<BHive::Framebuffer> mFramebuffer;
		Ref<BHive::Framebuffer> mLightingbuffer;
		Ref<BHive::PPlane> mScreenQuad;

		glm::ivec2 mViewportSize{};

		// PostProcessing
		Ref<BHive::Bloom> mBloom;
	};
} // namespace SolarSystem