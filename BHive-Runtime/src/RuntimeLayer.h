#pragma once

#include "core/Core.h"
#include "core/Layer.h"
#include "gfx/cameras/EditorCamera.h"
#include "core/events/ApplicationEvents.h"

namespace BHive
{
	class Texture2D;
	class Pipeline;

	class RuntimeLayer : public Layer
	{
	public:
		void OnAttach(Application& app) override;
		void OnDetach() override;
		void OnUpdate(float) override;
		void OnGuiRender() override;
		void OnEvent(Event &e) override;

	private:
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		Ref<Texture2D> mTexture = nullptr;
		Ref<class ShaderProgram> mShader = nullptr;
		Ref<class ShaderProgram> mEmissiveShader = nullptr;
		Ref<class Material> mMaterial = nullptr;
		Ref<class EmissiveMaterial> mEmissiveMaterial = nullptr;
		Ref<class LambertMaterial> mLambertMaterial = nullptr;
		Ref<class StaticMesh> mMesh = nullptr;
		Ref<Pipeline> mPipeline;
		Ref<Pipeline> mEmmissivePipeline;
		Ref<class Framebuffer> mFramebuffer;
		Ref<class GPUBuffer> mMultiDrawIndirectBuffer;
		EditorCamera mCamera;
	};
} // namespace BHive