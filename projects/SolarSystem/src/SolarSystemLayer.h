#pragma once

#include <gfx/cameras/EditorCamera.h>
#include <core/Application.h>
#include <core/Core.h>
#include <core/FPSCounter.h>
#include <core/Layer.h>

#include "renderer/CullingPipeline.h"
#include "renderer/UniverseRenderPipeline.h"
#include "ResourceManager.h"

BEGIN_NAMESPACE(BHive)

class UniformBuffer;
class Shader;
class Framebuffer;
class PQuad;
class Bloom;
class AudioSource;
class Font;
class Universe;

struct SolarSystemLayer : public Layer
{
	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(float dt) override;

	virtual void OnEvent(Event &e);

	virtual void OnGuiRender() override;

private:
	void InitFramebuffer();
	bool OnWindowResize(WindowResizeEvent &e);

private:
	Ref<Shader> mLightingShader;
	Ref<Shader> mQuadShader;

	EditorCamera mCamera;

	Ref<Framebuffer> mMultiSampleFramebuffer;
	Ref<Framebuffer> mFramebuffer;
	Ref<Framebuffer> mLightingbuffer;
	Ref<PQuad> mScreenQuad;

	// Ref<Framebuffer> mCullingBuffer;

	// PostProcessing
	Ref<Bloom> mBloom;

	Ref<Universe> mUniverse;
	Ref<ResourceManager> mResourceManager;

	Ref<AudioSource> mAudio;
	Ref<struct CelestrialBody> mPlayer;

	UniverseRenderPipeline mPipeline;
	// CullingPipeline mCullingPipeline;
};

END_NAMESPACE