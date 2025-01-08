#include "MeshColliderComponent.h"
#include "mesh/StaticMesh.h"
#include "scene/Entity.h"
#include "physics/PhysicsContext.h"
#include "physics/PhysicsUtils.h"
#include <reactphysics3d/reactphysics3d.h>
#include "renderers/LineRenderer.h"

namespace BHive
{

	void *MeshColliderComponent::GetCollisionShape(const FTransform &world_transform)
	{
		CreateConvexMesh();
		mCollisionShape = PhysicsContext::get_context().createConvexMeshShape(
			(rp3d::ConvexMesh*)mConvexMesh, physics::utils::vec3_to_rp3d(world_transform.get_scale()));

		return mCollisionShape;
	}

	void MeshColliderComponent::ReleaseCollisionShape()
	{
		PhysicsContext::get_context().destroyConvexMeshShape((rp3d::ConvexMeshShape *)mCollisionShape);
		PhysicsContext::get_context().destroyConvexMesh((rp3d::ConvexMesh *)mConvexMesh);
	}

	void MeshColliderComponent::OnRender(SceneRenderer *renderer)
	{		
		if (mStaticMesh)
		{
			auto transform = GetWorldTransform();
			LineRenderer::DrawAABB(mStaticMesh->GetBoundingBox(), mColor, transform);
		}
	}

	void MeshColliderComponent::SetStaticMesh(const TAssetHandle<StaticMesh> &mesh)
	{
		mStaticMesh = mesh;
	}

	void MeshColliderComponent::Serialize(StreamWriter &ar) const
	{
		ColliderComponent::Serialize(ar);
		ar(mStaticMesh);
	}

	void MeshColliderComponent::Deserialize(StreamReader &ar)
	{
		ColliderComponent::Deserialize(ar);
		ar(mStaticMesh);
	}

	void MeshColliderComponent::CreateConvexMesh()
	{
		if (!mConvexMesh)
		{
			auto &mesh_data = mStaticMesh->GetData();
			auto &vertices = mesh_data.mVertices;
			/*auto &indices = mesh_data.mIndices;
			auto num_faces = vertices.size() / 4;

			rp3d::PolygonVertexArray::PolygonFace *faces =
				new rp3d::PolygonVertexArray::PolygonFace[num_faces];

			rp3d::PolygonVertexArray::PolygonFace *face = faces;
			for (int i = 0; i < num_faces; i++)
			{
				face->indexBase = i * 6;
				face->nbVertices = 4;
				face++;
			}

			auto polys = rp3d::PolygonVertexArray(
				vertices.size(), vertices.data(), sizeof(FVertex), indices.data(),
				sizeof(uint32_t), num_faces, faces,
				rp3d::PolygonVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
				rp3d::PolygonVertexArray::IndexDataType::INDEX_INTEGER_TYPE);*/

			auto verts = rp3d::VertexArray(vertices.data(), sizeof(FVertex), vertices.size(),
											rp3d::VertexArray::DataType ::VERTEX_FLOAT_TYPE);

			std::vector<rp3d::Message> messages;
			mConvexMesh = PhysicsContext::get_context().createConvexMesh(verts, messages);

			if (messages.size())
			{
				for (auto &message : messages)
				{
					switch (message.type)
					{
						case reactphysics3d::Message::Type::Information:
							LOG_TRACE("StaticMeshComponent::GetConvexMesh - {}", message.text);
							break;
						case reactphysics3d::Message::Type::Warning:
							LOG_WARN("StaticMeshComponent::GetConvexMesh - {}", message.text);
							break;
						case reactphysics3d::Message::Type::Error:
							LOG_ERROR("StaticMeshComponent::GetConvexMesh - {}", message.text);
							break;
					}
				}
			}

			// delete[] faces;
		}
	}

	REFLECT(MeshColliderComponent)
	{
		BEGIN_REFLECT(MeshColliderComponent)
		(META_DATA(ClassMetaData_ComponentSpawnable, true)) REQUIRED_COMPONENT_FUNCS()
			REFLECT_PROPERTY("StaticMesh", GetStaticMesh, SetStaticMesh);
	}
}  // namespace BHive