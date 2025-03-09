#pragma once

#include "core/Layer.h"
#include "mesh/primitives/Sphere.h"
#include "mesh/primitives/Plane.h"
#include "gfx/textures/RenderTarget2D.h"
#include "gfx/Shader.h"
#include "mesh/indirect_mesh/IndirectMesh.h"
#include "core/events/ApplicationEvents.h"
#include "gfx/cameras/EditorCamera.h"

namespace BHive
{
	class ReflectionLayer : public Layer
	{
	public:
		void OnAttach() override;
		void OnUpdate(float dt) override;
		void OnEvent(Event &e) override;
		bool OnWindowResize(WindowResizeEvent &e);
		void DrawScene();

	private:
		Ref<RenderTarget2D> mRelfectionTarget;
		Ref<PSphere> mSphere;
		Ref<PPlane> mPlane;
		Ref<Shader> mReflectionShader;

		Ref<IndirectRenderable> mPlaneIndirect;
		Ref<IndirectRenderable> mSphereIndirect;
		EditorCamera mCamera;
	};
} // namespace BHive