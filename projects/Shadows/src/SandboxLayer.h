#pragma once

#include "core/Core.h"
#include "core/Layer.h"
#include "cameras/EditorCamera.h"
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

	class SandboxLayer : public Layer
	{
	public:
		virtual void OnAttach() override;
		virtual void OnUpdate(float dt) override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event &e) override;

	private:
		bool OnWindowResize(WindowResizeEvent &e);

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
	};
} // namespace BHive