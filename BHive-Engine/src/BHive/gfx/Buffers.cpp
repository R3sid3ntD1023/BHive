#include "Buffers.h"
#include "gfx/RenderCommand.h"

namespace BHive
{
	IndexBuffer::IndexBuffer(size_t _bufSize, EBufferLifetime lifeTime, const uint32_t *data, uint32_t count)
		: mCount(count)
	{
		RenderCommand::GetGraphicsAPI()->CreateBuffer(mBufferID, EBufferType::IndexBuffer, lifeTime, _bufSize);
		RenderCommand::GetGraphicsAPI()->SetBufferData(mBufferID, data, count * sizeof(uint32_t), 0);
	}

	IndexBuffer::~IndexBuffer()
	{
		RenderCommand::GetGraphicsAPI()->DeleteBuffer(mBufferID);
	}

	void IndexBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		RenderCommand::GetGraphicsAPI()->SetBufferData(mBufferID, data, size, offset);
	}

	void IndexBuffer::Clear()
	{
		RenderCommand::GetGraphicsAPI()->ClearBuffer(mBufferID);
	}

	VertexBuffer::VertexBuffer(size_t bufSize, EBufferLifetime lifeTime, const void *data, size_t size)
	{
		RenderCommand::GetGraphicsAPI()->CreateBuffer(mBufferID, EBufferType::VertexBuffer, lifeTime, bufSize);
		RenderCommand::GetGraphicsAPI()->SetBufferData(mBufferID, data, size, 0);
	}

	VertexBuffer::~VertexBuffer()
	{
		RenderCommand::GetGraphicsAPI()->DeleteBuffer(mBufferID);
	}

	void VertexBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		RenderCommand::GetGraphicsAPI()->SetBufferData(mBufferID, data, size, offset);
	}

	void VertexBuffer::Clear()
	{
		RenderCommand::GetGraphicsAPI()->ClearBuffer(mBufferID);
	}

	GeneralBuffer::GeneralBuffer(size_t bufSize, EBufferType type, EBufferLifetime lifeTime, const void *data, size_t size)
	{
		RenderCommand::GetGraphicsAPI()->CreateBuffer(mBufferID, type, lifeTime, bufSize);
		RenderCommand::GetGraphicsAPI()->SetBufferData(mBufferID, data, size, 0);
	}

	GeneralBuffer::~GeneralBuffer()
	{
		RenderCommand::GetGraphicsAPI()->DeleteBuffer(mBufferID);
	}

	void GeneralBuffer::SetData(const void *data, size_t size, uint32_t offset)
	{
		RenderCommand::GetGraphicsAPI()->SetBufferData(mBufferID, data, size, offset);
	}

	void GeneralBuffer::Clear()
	{
		RenderCommand::GetGraphicsAPI()->ClearBuffer(mBufferID);
	}

} // namespace BHive