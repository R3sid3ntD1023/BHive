#pragma once

#include "Buffers.h"
#include "gfx/registries/Handles.h"

namespace BHive
{
	struct FPass;

	class BHIVE_API VertexArray
	{
	public:
		virtual ~VertexArray() = default;

		virtual void SetIndexBuffer(IndexBufferPtr indexbuffer) = 0;

		virtual void AddVertexBuffer(VertexBufferPtr vertexbuffer) = 0;

		virtual IndexBufferPtr GetIndexBuffer() const = 0;

		virtual const std::vector<VertexBufferPtr> &GetVertexBuffers() const = 0;

		void DeclareAccess(FPass &pass, EBufferUsage vbAccess, EBufferUsage ibAccess);
	};
} // namespace BHive