#include "VertexArray.h"
#include <glad/glad.h>

namespace BHive
{
	VertexArray::VertexArray()
	{
		glCreateVertexArrays(1, &mVertexArrayID);
	}

	VertexArray::~VertexArray()
	{

		glDeleteVertexArrays(1, &mVertexArrayID);
	}

	void VertexArray::Bind() const
	{

		glBindVertexArray(mVertexArrayID);
	}

	void VertexArray::UnBind() const
	{

		glBindVertexArray(0);
	}

	void VertexArray::SetIndexBuffer(const Ref<IndexBuffer> &indexbuffer)
	{

		glVertexArrayElementBuffer(mVertexArrayID, indexbuffer->GetBufferID());

		mIndexBuffer = indexbuffer;
	}

	void VertexArray::AddVertexBuffer(const Ref<VertexBuffer> &vertexbuffer)
	{

		auto &layout = vertexbuffer->GetLayout();
		auto &elements = layout.GetElements();
		auto stride = layout.GetStride();

		if (elements.size() == 0)
			return;

		for (const auto &element : elements)
		{
			auto type = element.Type;
			auto divisor = element.Divisor;
			switch (type)
			{
			case BHive::EShaderDataType::Float:
			case BHive::EShaderDataType::Float2:
			case BHive::EShaderDataType::Float3:
			case BHive::EShaderDataType::Float4:
			{
				glEnableVertexArrayAttrib(mVertexArrayID, mVertexBufferIndex);
				glVertexArrayVertexBuffer(
					mVertexArrayID, mVertexBufferIndex, vertexbuffer->GetBufferID(), element.Offset, stride);
				mVertexBufferIndex++;
				break;
			}
			case BHive::EShaderDataType::Int:
			case BHive::EShaderDataType::Int2:
			case BHive::EShaderDataType::Int3:
			case BHive::EShaderDataType::Int4:
			case BHive::EShaderDataType::Bool:
			{
				glEnableVertexArrayAttrib(mVertexArrayID, mVertexBufferIndex);
				glVertexArrayVertexBuffer(
					mVertexArrayID, mVertexBufferIndex, vertexbuffer->GetBufferID(), element.Offset, stride);
				mVertexBufferIndex++;
				break;
			}
			case BHive::EShaderDataType::Mat3:
			case BHive::EShaderDataType::Mat4:
			{
				auto count = element.ComponentCount;
				for (uint8_t i = 0; i < count; i++)
				{
					glEnableVertexArrayAttrib(mVertexArrayID, mVertexBufferIndex);
					glVertexArrayVertexBuffer(
						mVertexArrayID, mVertexBufferIndex, vertexbuffer->GetBufferID(),
						element.Offset + sizeof(float) * count * i, stride);
					glVertexArrayBindingDivisor(mVertexArrayID, mVertexBufferIndex, 1);
					mVertexBufferIndex++;
				}
				break;
			}
			default:
				break;
			}
		}

		mVertexBuffers.push_back(vertexbuffer);
	}
	void VertexArray::BindBuffersBase(uint32_t binding) const
	{
		int i = binding;
		for (auto &vertexBuffer : mVertexBuffers)
		{
			vertexBuffer->BindBufferBase(i++);
		}
	}
} // namespace BHive