#pragma once

#include <cameras/EditorCamera.h>
#include <core/Application.h>
#include <core/Core.h>
#include <core/FPSCounter.h>
#include <core/Layer.h>

#include "ResourceManager.h"

namespace BHive
{
	class UniformBuffer;
	class Shader;
	class Framebuffer;
	class PPlane;
	class Bloom;
	class AudioSource;
	class Font;

} // namespace BHive

class Universe;

struct SolarSystemLayer : public BHive::Layer
{
	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(float dt) override;

	virtual void OnEvent(BHive::Event &e);

	virtual void OnGuiRender() override;

private:
	void InitFramebuffer();
	bool OnWindowResize(BHive::WindowResizeEvent &e);

private:
	Ref<BHive::Shader> mLightingShader;
	Ref<BHive::Shader> mQuadShader;

	BHive::EditorCamera mCamera;

	Ref<BHive::Framebuffer> mMultiSampleFramebuffer;
	Ref<BHive::Framebuffer> mFramebuffer;
	Ref<BHive::Framebuffer> mLightingbuffer;
	Ref<BHive::PPlane> mScreenQuad;

	Ref<BHive::Framebuffer> mCullingBuffer;
	// Ref<struct CullingSystem> mCullingSystem;

	// PostProcessing
	Ref<BHive::Bloom> mBloom;

	Ref<Universe> mUniverse;
	Ref<BHive::ResourceManager> mResourceManager;

	Ref<BHive::AudioSource> mAudio;
	Ref<struct CelestrialBody> mPlayer;
};