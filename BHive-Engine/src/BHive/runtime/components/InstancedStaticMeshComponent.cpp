#include "InstancedStaticMeshComponent.h"
#include "gfx/mesh/StaticMesh.h"
#include "gfx/renderers/SceneRenderer.h"
#include "runtime/GameObject.h"

namespace BHive
{
	void InstancedStaticMeshComponent::SetInstances(const std::vector<FTransform> &transforms)
	{
		mInstances = transforms;
		MarkDirty();
	}

	void InstancedStaticMeshComponent::OnRender(const FComponentRenderContext &context)
	{
		auto mesh = GetStaticMesh();
		if (!mesh)
			return;

		FMeshSubmissionRequest request{};
		request.Mesh = mesh.Get();
		request.Materials = GetMaterials();
		request.Transform = GetWorldTransform();
		request.EntityID = context.EntityID;
		request.InstanceTransforms = GetInstances();
		context.Scene.SubmitMesh(request);
	}

	void InstancedStaticMeshComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		StaticMeshComponent::Save(ar);
		ar(mInstances);
	}

	void InstancedStaticMeshComponent::Load(cereal::BinaryInputArchive &ar)
	{
		StaticMeshComponent::Load(ar);
		ar(mInstances);
		MarkDirty();
	}

	REFLECT(InstancedStaticMeshComponent)
	{
		BEGIN_REFLECT(InstancedStaticMeshComponent)(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() COMPONENT_IMPL();
	}
} // namespace BHive