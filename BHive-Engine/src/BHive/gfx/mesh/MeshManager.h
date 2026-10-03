#pragma once

#include "MeshData.h"
#include "gfx/registries/Handles.h"

namespace BHive
{
	struct FMeshGeometry
	{
		VertexArrayPtr VertexArray;
		uint32_t Page = 0;
		uint32_t BaseVertex = 0;
		uint32_t BaseIndex = 0;
	};

	class BHIVE_API MeshManager
	{
	public:
		static MeshManager &Get();

		FMeshGeometry Register(const FMeshData &data);

	private:
		struct Page
		{
			VertexBufferPtr VertexBuffer;
			IndexBufferPtr IndexBuffer;
			VertexArrayPtr VertexArray;
			uint32_t VertexCount = 0;
			uint32_t IndexCount = 0;
			uint32_t VertexCapacity = 0;
			uint32_t IndexCapacity = 0;
		};

		Page &CreatePage(uint32_t vertexCount, uint32_t indexCount);

		std::vector<Page> mPages;
	};
} // namespace BHive