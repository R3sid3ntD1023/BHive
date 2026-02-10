#pragma once

#include "core/Core.h"
#include "core/Layer.h"
#include "gfx/cameras/EditorCamera.h"
#include "core/events/ApplicationEvents.h"

namespace BHive
{
	class Texture2D;

	class RuntimeLayer : public Layer
	{
	public:
		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(float) override;
		void OnGuiRender() override;
		void OnEvent(Event &e) override;

	private:
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		Ref<Texture2D> mTexture = nullptr;

		Ref<class StaticMesh> mMesh = nullptr;

		Ref<class Shader> mShader;
		Ref<class Material> mMaterial;
		EditorCamera mCamera;
	};
} // namespace BHive