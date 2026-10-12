#pragma once

#include "core/Core.h"
#include "core/events/ApplicationEvents.h"
#include "core/events/KeyEvents.h"
#include "core/Layer.h"
#include "gfx/cameras/EditorCamera.h"
#include "gfx/cameras/EditorCameraController.h"
#include "gfx/material/MaterialTable.h"

namespace BHive
{
	class Texture2D;
	class BaseMesh;
	class SkeletalMesh;
	class SceneRenderer;
	class Font;
	class SkeletalAnimation;
	class Skeleton;
	class SkeletalPose;
	class AnimationClip;

	class SceneLayer : public Layer
	{
	public:
		void OnAttach(Application &app) override;

		void OnDetach() override;

		void OnUpdate(float) override;

		void OnRender(Renderer &renderer) override;

		void OnGuiRender() override;

		void OnEvent(Event &e);

		bool OnKeyEvent(KeyEvent &e);

	private:
		Ref<SceneRenderer> mSceneRenderer;
		Ref<BaseMesh> mMesh;
		Ref<BaseMesh> mPlane;
		std::vector<MaterialTable> mMaterialTables;
		EditorCamera mCameras[2];
		glm::uvec2 mViewportSize{0, 0};
		bool mViewportActive = false;
		EditorCameraController mCameraController;
		uint32_t mCurrentCameraIndex = 0;
		Ref<SkeletalMesh> mCharacter;

		Ref<class SkeletalAnimation> mCharacterAnimaton;
		Ref<class Skeleton> mCharacterSkeleton;
		Ref<class SkeletalPose> mCharacterPose;
		Ref<class AnimationClip> mAnimationClip;

		Ref<Texture2D> mTexture;
		Ref<Texture2D> mMarioTex;
		Ref<Font> mFont;
	};

} // namespace BHive