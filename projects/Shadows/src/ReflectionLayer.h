#pragma once

#include "core/Layer.h"
#include "mesh/primitives/Sphere.h"
#include "mesh/primitives/Plane.h"
#include "gfx/Shader.h"
#include "mesh/indirect_mesh/IndirectMesh.h"
#include "core/events/ApplicationEvents.h"
#include "gfx/cameras/EditorCamera.h"

namespace BHive
{
	class RenderTargetCube;
	class UniformBuffer;

	class ReflectionLayer : public Layer
	{
	public:
		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(float dt) override;
		void OnEvent(Event &e) override;
		bool OnWindowResize(WindowResizeEvent &e);
		void DrawScene();

	private:
		Ref<RenderTargetCube> mRelfectionTarget;
		Ref<PSphere> mSphere;
		Ref<PPlane> mPlane;
		Ref<Shader> mReflectionShader;

		Ref<IndirectRenderable> mPlaneIndirect;
		Ref<IndirectRenderable> mSphereIndirect;
		EditorCamera mCamera;
		Ref<UniformBuffer> mReflectUBO;
	};
} // namespace BHive