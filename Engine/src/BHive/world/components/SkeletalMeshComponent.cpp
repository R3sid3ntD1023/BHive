#include "SkeletalMeshComponent.h"
#include "mesh/SkeletalMesh.h"
#include "World/GameObject.h"
#include "Renderers/Renderer.h"

namespace BHive
{
	void SkeletalMeshComponent::Render()
	{

		if (SkeletalMeshAsset)
		{
			auto pose = SkeletalMeshAsset->GetDefaultPose();
			auto owner = GetOwner();
			auto world_transform = owner ? owner->GetWorldTransform() : FTransform{};
			MeshRenderer::DrawMesh(SkeletalMeshAsset, *pose, world_transform);
			LineRenderer::DrawAABB(SkeletalMeshAsset->GetBoundingBox(), Colors::Red, world_transform);

			const auto &joints = pose->GetTransformsJointSpace();

			for (size_t i = 0; i < joints.size(); i++)
			{
				const glm::mat4 parent = world_transform * joints[i];
				LineRenderer::DrawJoint(parent, 1.0f);

				if (i == joints.size() - 1)
					continue; // skip last joint, as it has no child

				const glm::mat4 child = world_transform * joints[i + 1];
				LineRenderer::DrawLine(parent[3], child[3], Colors::Cyan, world_transform);
			}
		}
	}

	void SkeletalMeshComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(TAssetHandle(SkeletalMeshAsset));
	}

	void SkeletalMeshComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(TAssetHandle(SkeletalMeshAsset));
	}

	REFLECT(SkeletalMeshComponent)
	{
		BEGIN_REFLECT(SkeletalMeshComponent)
		(META_DATA(EClassMetaData::ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR()
			REFLECT_PROPERTY("Skeletal Mesh Asset", SkeletalMeshAsset) COMPONENT_IMPL();
	}
} // namespace BHive