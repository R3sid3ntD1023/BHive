#include "StaticMeshComponent.h"
#include "gfx/mesh/StaticMesh.h"
#include "gfx/renderers/SceneRenderer.h"
#include "runtime/GameObject.h"

namespace BHive
{

	void StaticMeshComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(mOverrideMaterials, mStaticMeshAsset);
	}

	void StaticMeshComponent::Load(cereal::BinaryInputArchive &ar)
	{
		ar(mOverrideMaterials, mStaticMeshAsset);
		MarkDirty();
	}

	void StaticMeshComponent::SetStaticMesh(AssetHandle<StaticMesh> mesh)
	{
		const auto materialCount = mesh ? mesh->GetMaterialCount() : 0;

		mStaticMeshAsset = mesh;
		mOverrideMaterials.Resize(materialCount);
		mHasSubmittedMesh = false;
		MarkDirty();
	}

	void StaticMeshComponent::SetMaterial(uint32_t index, AssetHandle<Material> material)
	{
		mOverrideMaterials.Set(index, material.Get());
		mHasSubmittedMaterials = false;
		MarkDirty();
	}

	void StaticMeshComponent::OnRender(const FComponentRenderContext &context)
	{
		auto mesh = GetStaticMesh();
		if (!mesh)
			return;

		if (!mHasSubmittedMesh)
		{
			if (mRenderContext.ContextIndex != -1)
			{
				context.Scene.UpdateMesh(mRenderContext, nullptr);
			}

			FMeshSubmissionRequest request{};
			request.Mesh = mesh.Get();
			request.Materials = GetMaterials();
			request.Transform = GetWorldTransform();
			request.EntityID = context.EntityID;
			context.Scene.SubmitMesh(request, mRenderContext);
			mHasSubmittedMesh = true;
			mHasSubmittedMaterials = true;
		}
		else if (context.IsDirty)
		{
			context.Scene.UpdateTransform(mRenderContext, GetWorldTransform());
		}

		if (!mHasSubmittedMaterials)
		{
			if (mRenderContext.ContextIndex != -1)
			{
				context.Scene.UpdateMaterials(mRenderContext, GetMaterials());
			}
			mHasSubmittedMaterials = true;
		}
	}

	REFLECT(StaticMeshComponent)
	{
		BEGIN_REFLECT(StaticMeshComponent)(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY("Material Overrides", mOverrideMaterials)
			REFLECT_PROPERTY("StaticMesh", GetStaticMesh, SetStaticMesh) COMPONENT_IMPL();
	}

} // namespace BHive
