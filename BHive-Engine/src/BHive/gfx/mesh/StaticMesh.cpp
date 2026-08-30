#include "gfx/VertexArray.h"
#include "StaticMesh.h"

namespace BHive
{
	StaticMesh::StaticMesh(const FMeshData &data)
		: BaseMesh(data)
	{
	}

	void StaticMesh::Save(cereal::BinaryOutputArchive &ar) const
	{
		BaseMesh::Save(ar);
	}

	void StaticMesh::Load(cereal::BinaryInputArchive &ar)
	{
		BaseMesh::Load(ar);
	}

	REFLECT(StaticMesh)
	{
		BEGIN_REFLECT(StaticMesh)
		REFLECT_CONSTRUCTOR();
		rttr::type::register_wrapper_converter_for_base_classes<Ref<StaticMesh>>();
	}
} // namespace BHive