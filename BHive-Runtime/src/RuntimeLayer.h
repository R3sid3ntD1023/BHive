#pragma once

#include "core/Core.h"
#include "core/Layer.h"
#include "gfx/cameras/EditorCamera.h"
#include "core/events/ApplicationEvents.h"
#include "gfx/renderers/postprocess/PostProcessStack.h"

namespace BHive
{
	class Texture2D;
	class Pipeline;
	class PSphere;
	class PPlane;

	class RuntimeLayer : public Layer
	{
	public:
		void OnAttach(Application &app) override;

		void OnDetach() override;

		void OnUpdate(float) override;

		void OnRender(Renderer &renderer) override;

		void OnGuiRender() override;

		void OnEvent(Event &e) override;

	private:
		bool OnWindowResize(WindowResizeEvent &e);

	private:
		Ref<Texture2D> mTexture = nullptr;
		Ref<Texture2D> mEnvironmentTex;

		Ref<class Framebuffer> mFramebuffer;
		Ref<class GeneralBuffer> mMultiDrawIndirectBuffer;
		EditorCamera mCamera;
		Ref<GeneralBuffer> mModelBuffer = nullptr;

		Ref<PSphere> mSphere;
		Ref<PPlane> mPlane;
		Ref<class StaticMesh> mMesh = nullptr;

		Ref<class EmissiveMaterial> mEmissiveMaterial = nullptr;
		Ref<class LambertMaterial> mLambertMaterial = nullptr;
		Ref<class StandardMaterial> mStandardMaterial = nullptr;

		PostProcessAllocator mPostProcessAllocator;
		PostProcessStack mPostProcessStack;
		Ref<class Texture> mFinalSceneColor;
		Ref<class BloomMaterial> mBloomMaterial;
		Ref<class ColorGradingMaterial> mColorGrading;

		glm::uvec2 mViewportSize{0, 0};
	};
} // namespace BHive