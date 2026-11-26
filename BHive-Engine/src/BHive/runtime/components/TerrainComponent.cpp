#include "TerrainComponent.h"
#include "gfx/renderers/SceneRenderer.h"
#include "runtime/GameObject.h"

namespace BHive
{

	void TerrainComponent::Build()
	{
		mTerrain.Init(TerrainParameters);
		mTerrainMesh = CreateRef<StaticMesh>(mTerrain.BuildMesh());
		mIsMeshDirty = true;
	}

	void TerrainComponent::SetMaterial(AssetHandle<Material> material)
	{
		mMaterial.Set(0, material);
		mIsMaterialDirty = true;
	}

	void TerrainComponent::OnRender(const FComponentRenderContext &context)
	{
		if (!mTerrainMesh)
			return;

		if (mIsMaterialDirty && mRenderContext.ContextIndex != -1)
		{
			context.Scene.UpdateMaterials(mRenderContext, mMaterial);
			mIsMaterialDirty = false;
		}

		if (mIsMeshDirty)
		{
			if (mRenderContext.ContextIndex != -1)
			{
				context.Scene.UpdateMesh(mRenderContext, nullptr);
			}
			else
			{
				FMeshSubmissionRequest request{};
				request.Mesh = mTerrainMesh;
				request.Materials = mMaterial;
				request.Transform = GetWorldTransform();
				request.EntityID = context.EntityID;
				context.Scene.SubmitMesh(request, mRenderContext);
			}
			mIsMeshDirty = false;
		}
		else if (context.IsDirty)
		{
			context.Scene.UpdateTransform(mRenderContext, GetWorldTransform());
		}
	}

	void TerrainComponent::Save(cereal::BinaryOutputArchive &ar) const
	{
		ar(TerrainParameters, mMaterial, mTerrainMesh ? mTerrainMesh->GetData() : FMeshData{});
	}

	void TerrainComponent::Load(cereal::BinaryInputArchive &ar)
	{
		FMeshData data{};
		ar(TerrainParameters, mMaterial, data);

		mTerrainMesh = CreateRef<StaticMesh>(data);
		mTerrain.Init(TerrainParameters);
	}

	REFLECT(TerrainComponent)
	{
		BEGIN_REFLECT(TerrainComponent)(META_DATA(ClassMetaData_ComponentSpawnable, true)) REFLECT_CONSTRUCTOR() REFLECT_PROPERTY("Material", GetMaterial, SetMaterial)
			REFLECT_PROPERTY(TerrainParameters) COMPONENT_IMPL();
	}
} // namespace BHive