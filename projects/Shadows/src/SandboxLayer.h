#pragma once

#include "core/Core.h"
#include "core/Layer.h"
#include "gfx/cameras/EditorCamera.h"
#include "core/events/ApplicationEvents.h"

namespace BHive
{
	class IndirectRenderable;
	class PCube;
	class Shader;
	class StaticMesh;
	class Skeleton;
	class SkeletalMesh;
	class SkeletalAnimation;
	class AnimationClip;
	class SkeletalPose;
	class Framebuffer;
	class PQuad;

	class SandboxLayer : public Layer
	{
	public:
		virtual void OnAttach() override;
		virtual void OnUpdate(float dt) override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event &e) override;
		virtual void OnGuiRender() override;

	private:
		bool OnWindowResize(WindowResizeEvent &e);
		void DrawScene();

	private:
		EditorCamera mCamera;
		Ref<IndirectRenderable> mIndirectPlane;
		Ref<IndirectRenderable> mIndirectLightPost;
		Ref<IndirectRenderable> mIndirectCharacter;
		Ref<PCube> mPlane;
		Ref<StaticMesh> mLightPost;

		Ref<Skeleton> mSkeleton;
		Ref<SkeletalMesh> mCharacter;
		Ref<SkeletalAnimation> mAnimation;
		Ref<AnimationClip> mAnimationClip;
		Ref<Shader> mShader;
		Ref<SkeletalPose> mPose;

		Ref<Framebuffer> mFramebuffer;
		Ref<PQuad> mScreenQuad;
		Ref<Shader> mScreenQuadShader;
	};
} // namespace BHive