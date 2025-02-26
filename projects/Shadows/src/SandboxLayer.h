#pragma once

#include "core/Core.h"
#include "core/Layer.h"
#include "cameras/EditorCamera.h"
#include "core/events/ApplicationEvents.h"

namespace BHive
{
	class IndirectRenderable;
	class PPlane;
	class Shader;
	class StaticMesh;

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
		Ref<PPlane> mPlane;
		Ref<StaticMesh> mLightPost;
		Ref<Shader> mShader;
	};
} // namespace BHive