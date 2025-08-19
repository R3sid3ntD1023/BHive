#include "MeshRenderer.h"
#include "gfx/RenderCommand.h"
#include "material/Material.h"
#include "gfx/StorageBuffer.h"
#include "gfx/UniformBuffer.h"
#include "mesh/StaticMesh.h"
#include "mesh/SkeletalMesh.h"
#include "gfx/Shader.h"
#include "Renderers/Renderer.h"
#include "core/math/boundingbox/AABB.h"
#include "core/math/volumes/SphereVolume.h"
#include "material/BDRFMaterial.h"

#include "core/threading/Threading.h"

#define SSBO_INDEX_PER_OBJECT_BINDING 1
#define SSBO_INSTANCE_BINDING 2
#define SSBO_BONE_BINDING 3
#define MAX_BONES 200

namespace BHive
{

	struct FObjectData
	{
		glm::mat4 ModelMatrix;
		FSubMesh SubMesh;
		Ref<VertexArray> VertexArray;
		AABB Bounds;
		const SkeletalPose *Pose = nullptr;

		// instances
		const glm::mat4 *Instances = nullptr;
		size_t InstanceCount = 0;
	};

	struct MeshRenderData
	{
		std::unordered_map<Ref<Material>, std::vector<FObjectData>> ObjectData;
		Ref<StorageBuffer> BoneBuffer;
		Ref<StorageBuffer> WorldMatrixBuffer;
		Ref<StorageBuffer> InstanceBuffer;
	};

	static MeshRenderData *sMeshRenderData = nullptr;

	void MeshRenderer::DrawMeshes()
	{
		for (auto &[mat, objects] : sMeshRenderData->ObjectData)
		{
			for (auto &[transform, submesh, vao, bounds, pose, instances, instanceCount] : objects)
			{
				auto matrix = transform * submesh.Transformation;
				sMeshRenderData->WorldMatrixBuffer->SetData(&matrix, sizeof(glm::mat4));
				sMeshRenderData->WorldMatrixBuffer->BindBufferBase(SSBO_INDEX_PER_OBJECT_BINDING);

				if (instanceCount > 0)
				{
					sMeshRenderData->InstanceBuffer->SetData(instances, sizeof(glm::mat4) * instanceCount);
					sMeshRenderData->InstanceBuffer->BindBufferBase(SSBO_INSTANCE_BINDING);
				}

				if (pose)
				{
					const auto &joints = pose->GetTransformsJointSpace();
					sMeshRenderData->BoneBuffer->SetData(joints.data(), joints.size() * sizeof(glm::mat4));
					sMeshRenderData->BoneBuffer->BindBufferBase(SSBO_BONE_BINDING);
				}

				RenderCommand::DrawElementsBaseVertex(EDrawMode::Triangles, *vao, submesh.StartVertex, submesh.StartIndex, submesh.IndexCount, instanceCount);
			}
		}
	}

	void MeshRenderer::Init()
	{
		sMeshRenderData = new MeshRenderData();
		sMeshRenderData->BoneBuffer = CreateRef<StorageBuffer>(sizeof(glm::mat4) * MAX_BONES);
		sMeshRenderData->WorldMatrixBuffer = CreateRef<StorageBuffer>(sizeof(glm::mat4));
		sMeshRenderData->InstanceBuffer = CreateRef<StorageBuffer>(sizeof(glm::mat4) * 1000);
	}

	void MeshRenderer::Shutdown()
	{
		delete sMeshRenderData;
	}

	void MeshRenderer::Begin()
	{
		ASSERT(sMeshRenderData);

		sMeshRenderData->ObjectData.clear();
	}

	void MeshRenderer::End()
	{
		ASSERT(sMeshRenderData);

		SortObjects();

		// display only visible meshes
		for (auto &[material, objects] : sMeshRenderData->ObjectData)
		{
			auto shader = material->GetShader();
			shader->Bind();
			material->Submit(shader);

			for (auto &[transform, submesh, vao, bounds, pose, instances, instanceCount] : objects)
			{
				auto matrix = transform * submesh.Transformation;
				sMeshRenderData->WorldMatrixBuffer->SetData(&matrix, sizeof(glm::mat4));
				sMeshRenderData->WorldMatrixBuffer->BindBufferBase(SSBO_INDEX_PER_OBJECT_BINDING);

				if (instanceCount > 0)
				{
					sMeshRenderData->InstanceBuffer->SetData(instances, sizeof(glm::mat4) * instanceCount);
					sMeshRenderData->InstanceBuffer->BindBufferBase(SSBO_INSTANCE_BINDING);
				}

				if (pose)
				{
					const auto &joints = pose->GetTransformsJointSpace();
					sMeshRenderData->BoneBuffer->SetData(joints.data(), joints.size() * sizeof(glm::mat4));
					sMeshRenderData->BoneBuffer->BindBufferBase(SSBO_BONE_BINDING);
				}

				RenderCommand::DrawElementsBaseVertex(EDrawMode::Triangles, *vao, submesh.StartVertex, submesh.StartIndex, submesh.IndexCount, instanceCount);
			}
		}
	}

	void MeshRenderer::DrawMesh(const Ref<StaticMesh> &mesh, const glm::mat4 &transform, const glm::mat4 *instances, size_t instanceCount)
	{
		DrawMesh(mesh, mesh->GetMaterialTable(), transform, instances, instanceCount);
	}

	void MeshRenderer::DrawMesh(const Ref<SkeletalMesh> &mesh, const SkeletalPose &pose, const glm::mat4 &transform, const glm::mat4 *instances, size_t instanceCount)
	{
		DrawMesh(mesh, mesh->GetMaterialTable(), pose, transform, instances, instanceCount);
	}

	void MeshRenderer::DrawMesh(const Ref<StaticMesh> &mesh, const MaterialTable &materials, const glm::mat4 &transform, const glm::mat4 *instances, size_t instanceCount)
	{
		// TODO: CullMeshes
		ASSERT(sMeshRenderData);

		if (!mesh || IsMeshCulled(mesh, transform))
			return;

		auto &sub_meshes = mesh->GetSubMeshes();

		for (auto &sub_mesh : sub_meshes)
		{
			auto material = materials.get_material(sub_mesh.MaterialIndex);
			if (!material)
				return;

			auto data = FObjectData{
				.ModelMatrix = transform, .SubMesh = sub_mesh, .VertexArray = mesh->GetVertexArray(), .Bounds = mesh->GetBoundingBox(), .Instances = instances, .InstanceCount = instanceCount};

			sMeshRenderData->ObjectData[material].emplace_back(data);
		}
	}

	void MeshRenderer::DrawMesh(const Ref<SkeletalMesh> &mesh, const MaterialTable &materials, const SkeletalPose &pose, const glm::mat4 &transform, const glm::mat4 *instances, size_t instanceCount)
	{
		// TODO: CullMeshes
		ASSERT(sMeshRenderData);

		if (!mesh || IsMeshCulled(mesh, transform))
			return;

		auto &sub_meshes = mesh->GetSubMeshes();
		for (auto &sub_mesh : sub_meshes)
		{
			auto material = materials.get_material(sub_mesh.MaterialIndex);
			if (!material)
				return;

			auto data = FObjectData{
				.ModelMatrix = transform,
				.SubMesh = sub_mesh,
				.VertexArray = mesh->GetVertexArray(),
				.Bounds = mesh->GetBoundingBox(),
				.Pose = &pose,
				.Instances = instances,
				.InstanceCount = instanceCount};

			sMeshRenderData->ObjectData[material].emplace_back(data);
		}
	}

	bool MeshRenderer::IsMeshCulled(const Ref<BaseMesh> &mesh, const glm::mat4 &transform)
	{
		if (!mesh)
			return true;

		const auto &bounds = mesh->GetBoundingBox();
		const auto &frustum = Renderer::GetFrustum();

		auto volume = FSphereVolume(bounds.GetCenter(), bounds.GetRadius());
		return !volume.InFrustum(frustum, FTransform(transform));
	}

	void MeshRenderer::SortObjects()
	{
		static auto sorter = [=](const FObjectData &a, const FObjectData &b)
		{
			const auto &A = a.ModelMatrix[3];
			const auto &B = b.ModelMatrix[3];
			const auto &C = CameraBuffer::Get().GetCameraData().CameraPosition;

			const auto distA = glm::distance(A, C);
			const auto distB = glm::distance(B, C);

			return distA < distB;
		};

		for (auto &[mat, objdatas] : sMeshRenderData->ObjectData)
		{
			auto &objects = objdatas;
			std::sort(objects.begin(), objects.end(), sorter);
		}
	}
} // namespace BHive