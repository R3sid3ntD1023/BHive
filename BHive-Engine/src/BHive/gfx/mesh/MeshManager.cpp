#include "MeshManager.h"
#include "gfx/Buffers.h"
#include "gfx/VertexArray.h"
#include "gfx/factories/BufferFactory.h"

namespace BHive
{
	namespace
	{
		constexpr uint32_t InitialVertexCapacity = 65536;
		constexpr uint32_t InitialIndexCapacity = 262144;

		uint32_t GetCapacity(size_t requested, uint32_t initial)
		{
			size_t capacity = initial;
			while (capacity < requested)
			{
				capacity *= 2;
				if (capacity > UINT32_MAX)
					throw std::length_error("Mesh geometry page exceeds 32-bit capacity");
			}
			return static_cast<uint32_t>(capacity);
		}
	} // namespace

	MeshManager &MeshManager::Get()
	{
		static MeshManager manager;
		return manager;
	}

	MeshManager::Page &MeshManager::CreatePage(uint32_t vertexCount, uint32_t indexCount)
	{
		Page page{};
		page.VertexCapacity = GetCapacity(vertexCount, InitialVertexCapacity);
		page.IndexCapacity = GetCapacity(indexCount, InitialIndexCapacity);

		const size_t vertexBytes = static_cast<size_t>(page.VertexCapacity) * sizeof(FVertex);
		const size_t indexBytes = static_cast<size_t>(page.IndexCapacity) * sizeof(uint32_t);
		if (vertexBytes > UINT32_MAX || indexBytes > UINT32_MAX)
			throw std::length_error("Mesh geometry page byte offsets exceed 32-bit buffer API limits");

		page.VertexBuffer = BufferFactory::CreateVertexBuffer(vertexBytes, EBufferLifetime::Static);
		page.VertexBuffer.As<VertexBuffer>()->SetLayout(FVertex::Layout());
		page.IndexBuffer = BufferFactory::CreateIndexBuffer(page.IndexCapacity, EBufferLifetime::Static);
		page.VertexArray = VertexArrayFactory::Create({page.VertexBuffer}, page.IndexBuffer);

		mPages.emplace_back(std::move(page));
		return mPages.back();
	}

	FMeshGeometry MeshManager::Register(const FMeshData &data)
	{
		if (data.Vertices.empty() && data.Indices.empty())
			return {};
		if (data.Vertices.empty() || data.Indices.empty())
			throw std::invalid_argument("Indexed mesh geometry requires both vertices and indices");

		if (data.Vertices.size() > INT32_MAX || data.Indices.size() > UINT32_MAX)
			throw std::length_error("Mesh geometry exceeds indirect draw offset limits");

		Page *page = nullptr;
		if (!mPages.empty())
		{
			auto &last = mPages.back();
			if (data.Vertices.size() <= last.VertexCapacity - last.VertexCount && data.Indices.size() <= last.IndexCapacity - last.IndexCount)
				page = &last;
		}

		if (!page)
			page = &CreatePage(static_cast<uint32_t>(data.Vertices.size()), static_cast<uint32_t>(data.Indices.size()));

		const uint32_t baseVertex = page->VertexCount;
		const uint32_t baseIndex = page->IndexCount;
		const size_t vertexBytes = data.Vertices.size() * sizeof(FVertex);
		const size_t indexBytes = data.Indices.size() * sizeof(uint32_t);

		if (vertexBytes)
			page->VertexBuffer.As<VertexBuffer>()->SetData(data.Vertices.data(), vertexBytes, baseVertex * sizeof(FVertex));
		if (indexBytes)
			page->IndexBuffer.As<IndexBuffer>()->SetData(data.Indices.data(), indexBytes, baseIndex * sizeof(uint32_t));

		page->VertexCount += static_cast<uint32_t>(data.Vertices.size());
		page->IndexCount += static_cast<uint32_t>(data.Indices.size());
		page->IndexBuffer.As<IndexBuffer>()->SetCount(page->IndexCount);

		return {page->VertexArray, static_cast<uint32_t>(mPages.size() - 1), baseVertex, baseIndex};
	}
} // namespace BHive