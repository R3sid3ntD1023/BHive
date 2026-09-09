#pragma once

#include "BaseMesh.h"
#include "core/Core.h"

namespace BHive
{
	class BHIVE_API StaticMesh : public BaseMesh
	{
	public:
		StaticMesh() = default;
		StaticMesh(const FMeshData &data);

		virtual void Save(cereal::BinaryOutputArchive &ar) const override;

		virtual void Load(cereal::BinaryInputArchive &ar) override;

		REFLECTABLEV(BaseMesh)
	};

	REFLECT_EXTERN(StaticMesh)
} // namespace BHive