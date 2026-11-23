#include "GLVertexArray.h"
#include "gfx/Buffers.h"
#include <glad/glad.h>

namespace BHive
{
	namespace
	{
		GLenum ToGLVertexType(EShaderDataType type)
		{
			switch (type)
			{
			case EShaderDataType::Float:
			case EShaderDataType::Float2:
			case EShaderDataType::Float3:
			case EShaderDataType::Float4:
			case EShaderDataType::Mat3:
			case EShaderDataType::Mat4:
				return GL_FLOAT;
			case EShaderDataType::UInt:
				return GL_UNSIGNED_INT;
			case EShaderDataType::Int:
			case EShaderDataType::Int2:
			case EShaderDataType::Int3:
			case EShaderDataType::Int4:
				return GL_INT;
			case EShaderDataType::Bool:
				return GL_UNSIGNED_BYTE;
			}
			return GL_FLOAT;
		}

		bool IsIntegerVertexType(EShaderDataType type)
		{
			return type == EShaderDataType::UInt || type == EShaderDataType::Int || type == EShaderDataType::Int2 ||
				type == EShaderDataType::Int3 || type == EShaderDataType::Int4 || type == EShaderDataType::Bool;
		}

		void ConfigureAttribute(
			GLuint vertexArray,
			GLuint buffer,
			GLuint attribute,
			GLuint binding,
			GLsizei stride,
			GLintptr bufferOffset,
			GLint componentCount,
			const BufferElement &element)
		{
			glEnableVertexArrayAttrib(vertexArray, attribute);
			glVertexArrayVertexBuffer(vertexArray, binding, buffer, bufferOffset, stride);
			glVertexArrayAttribBinding(vertexArray, attribute, binding);
			glVertexArrayBindingDivisor(vertexArray, binding, element.Divisor);

			const GLenum type = ToGLVertexType(element.Type);
			if (IsIntegerVertexType(element.Type) && !element.Normalized)
				glVertexArrayAttribIFormat(vertexArray, attribute, componentCount, type, 0);
			else
				glVertexArrayAttribFormat(vertexArray, attribute, componentCount, type, element.Normalized ? GL_TRUE : GL_FALSE, 0);
		}
	} // namespace

	GLVertexArray::GLVertexArray()
		: mVertexAttributeIndex(0),
		  mRendererID(0)
	{
		glCreateVertexArrays(1, &mRendererID);
	}

	GLVertexArray::GLVertexArray(const std::vector<VertexBufferPtr> &vertex_buffers, IndexBufferPtr index_buffer)
		: mVertexAttributeIndex(0),
		  mRendererID(0)
	{
		glCreateVertexArrays(1, &mRendererID);

		for (auto vb : vertex_buffers)
			AddVertexBuffer(vb);

		SetIndexBuffer(index_buffer);
	}

	void GLVertexArray::SetIndexBuffer(IndexBufferPtr indexbuffer)
	{
		mIndexBuffer = indexbuffer;
		glVertexArrayElementBuffer(mRendererID, indexbuffer ? indexbuffer.As<IndexBuffer>()->GetBufferID() : 0);
	}

	void GLVertexArray::Bind() const
	{
		glBindVertexArray(mRendererID);
	}

	void GLVertexArray::AddVertexBuffer(VertexBufferPtr vertexbuffer)
	{
		mVertexBuffers.emplace_back(vertexbuffer);
		CreateBindingsAndAttributes(vertexbuffer);
	}

	void GLVertexArray::CreateBindingsAndAttributes(VertexBufferPtr vertexbuffer)
	{
		auto vb = vertexbuffer.As<VertexBuffer>();
		auto &layout = vb->GetLayout();
		auto &elements = layout.GetElements();
		auto stride = layout.GetStride();

		if (elements.size() == 0)
			return;

		for (const auto &element : elements)
		{
			switch (element.Type)
			{
			case BHive::EShaderDataType::Float:
			case BHive::EShaderDataType::Float2:
			case BHive::EShaderDataType::Float3:
			case BHive::EShaderDataType::Float4:
			case BHive::EShaderDataType::UInt:
			case BHive::EShaderDataType::Int:
			case BHive::EShaderDataType::Int2:
			case BHive::EShaderDataType::Int3:
			case BHive::EShaderDataType::Int4:
			case BHive::EShaderDataType::Bool:
			{
				const GLuint attribute = mVertexAttributeIndex++;
				ConfigureAttribute(
					mRendererID,
					vb->GetBufferID(),
					attribute,
					attribute,
					static_cast<GLsizei>(stride),
					static_cast<GLintptr>(element.Offset),
					element.ComponentCount,
					element);
				break;
			}
			case BHive::EShaderDataType::Mat3:
			case BHive::EShaderDataType::Mat4:
			{
				const auto columnCount = element.ComponentCount;
				for (uint8_t i = 0; i < columnCount; i++)
				{
					const GLuint attribute = mVertexAttributeIndex++;
					const auto columnOffset = element.Offset + sizeof(float) * columnCount * i;
					ConfigureAttribute(
						mRendererID,
						vb->GetBufferID(),
						attribute,
						attribute,
						static_cast<GLsizei>(stride),
						static_cast<GLintptr>(columnOffset),
						columnCount,
						element);
				}
				break;
			}
			default:
				break;
			}
		}
	}
} // namespace BHive