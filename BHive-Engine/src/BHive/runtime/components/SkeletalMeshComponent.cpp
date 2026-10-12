#include "SkeletalMeshComponent.h"
#include "gfx/mesh/SkeletalMesh.h"
#include "gfx/renderers/SceneRenderer.h"
#include "runtime/GameObject.h"

namespace BHive
{

	void SkeletalMeshComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(mOverrideMaterials);
	}

	void SkeletalMeshComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(mOverrideMaterials);
		MarkDirty();
	}

	void SkeletalMeshComponent::SetSkeletalMesh(AssetHandle<SkeletalMesh> mesh)
	{
		SkeletalMeshAsset = mesh;
		mOverrideMaterials.Resize(mesh ? mesh->GetMaterialCount() : 0);
		MarkDirty();
	}

	void SkeletalMeshComponent::OnRender(const FComponentRenderContext &context)
	{
		auto mesh = GetSkeletalMesh();
		if (!mesh)
			return;

		auto pose = mesh->GetDefaultPose();
		if (!pose)
			return;

		FMeshSubmissionRequest request{};
		request.Mesh = mesh.Get();
		request.Materials = GetMaterials();
		request.Transform = GetWorldTransform();
		request.EntityID = context.EntityID;
		request.BoneTransforms = pose->GetTransformsJointSpace();
		context.Scene.SubmitMesh(request);
	}

	REFLECT(SkeletalMeshComponent)
	{
		BEGIN_REFLECT(SkeletalMeshComponent)
		(META_DATA(EClassMetaData::ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY("Skeletal Mesh", GetSkeletalMesh, SetSkeletalMesh)
			COMPONENT_IMPL();
	}
} // namespace BHive