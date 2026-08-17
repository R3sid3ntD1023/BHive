#pragma once

#include "EditorCamera.h"
#include "input/InputManager.h"

namespace BHive
{
	class BHIVE_API EditorCameraController
	{
	public:
		void SetCamera(EditorCamera *camera) { mCamera = camera; }

		void SetSelection(const glm::vec3 &position, const glm::vec3 &bounds)
		{
			mSelectedTransform = position;
			mSelectedBounds = bounds;
		}

		void Update(float dt);

	private:
		EditorCamera *mCamera = nullptr;

		glm::vec3 mSelectedTransform{};
		glm::vec3 mSelectedBounds{1.f};
	};
} // namespace BHive