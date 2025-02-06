#include "GLVertexArray.h"
#include <glad/glad.h>

namespace BHive
{
	GLVertexArray::GLVertexArray()
	{

		glCreateVertexArrays(1, &mVertexArrayID);
	}

	GLVertexArray::~GLVertexArray()
	{

		glDeleteVertexArrays(1, &mVertexArrayID);
	}

	void GLVertexArray::Bind() const
	{

		glBindVertexArray(mVertexArrayID);
	}

	void GLVertexArray::UnBind() const
	{

		glBindVertexArray(0);
	}

	void GLVertexArray::SetIndexBuffer(const Ref<IndexBuffer> &indexbuffer)
	{
		Bind();
		indexbuffer->Bind();

		mIndexBuffer = indexbuffer;
	}

	void GLVertexArray::AddVertexBuffer(const Ref<VertexBuffer> &vertexbuffer)
	{
		auto &layout = vertexbuffer->GetLayout();
		auto &elements = layout.GetElements();
		auto stride = layout.GetStride();

		if (elements.size() == 0)
			return;

		Bind();
		vertexbuffer->Bind();

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

				glEnableVertexAttribArray(mVertexBufferIndex);
				glVertexAttribPointer(mVertexBufferIndex, element.ComponentCount, GL_FLOAT, element.Normalized, stride, (const void *)element.Offset);
				if (divisor)
				{
					glVertexAttribDivisor(mVertexBufferIndex, 1);
				}

				mVertexBufferIndex++;
				break;
			}
			case BHive::EShaderDataType::Int:
			case BHive::EShaderDataType::Int2:
			case BHive::EShaderDataType::Int3:
			case BHive::EShaderDataType::Int4:
			case BHive::EShaderDataType::Bool:
			{
				glEnableVertexAttribArray(mVertexBufferIndex);
				glVertexAttribIPointer(mVertexBufferIndex, element.ComponentCount, GL_INT, stride, (const void *)element.Offset);
				if (divisor)
				{
					glVertexAttribDivisor(mVertexBufferIndex, 1);
				}
				mVertexBufferIndex++;
				break;
			}
			case BHive::EShaderDataType::Mat3:
			case BHive::EShaderDataType::Mat4:
			{
				auto count = element.ComponentCount;
				for (uint8_t i = 0; i < count; i++)
				{
					glEnableVertexAttribArray(mVertexBufferIndex);
					glVertexAttribPointer(
						mVertexBufferIndex, element.ComponentCount, GL_FLOAT, element.Normalized, stride,
						(const void *)(i * count * sizeof(float) + element.Offset));
					glVertexAttribDivisor(mVertexBufferIndex, 1);
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
} // namespace BHive