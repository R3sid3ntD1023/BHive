#include "VertexArray.h"
#include <glad/glad.h>

namespace BHive
{
	VertexArray::VertexArray()
	{
	}

	VertexArray::~VertexArray()
	{
	}

	void VertexArray::Bind() const
	{
	}

	void VertexArray::UnBind() const
	{
	}

	void VertexArray::SetIndexBuffer(const Ref<IndexBuffer> &indexbuffer)
	{
		mIndexBuffer = indexbuffer;
	}

	void VertexArray::AddVertexBuffer(const Ref<VertexBuffer> &vertexbuffer)
	{

		auto &layout = vertexbuffer->GetLayout();
		auto &elements = layout.GetElements();
		auto stride = layout.GetStride();

		if (elements.size() == 0)
			return;

		mBinding = vk::VertexInputBindingDescription(0, stride, vk::VertexInputRate::eVertex);

		for (const auto &element : elements)
		{
			auto type = element.Type;
			auto divisor = element.Divisor;
			switch (type)
			{
			case BHive::EShaderDataType::Float:
				mAttributes.emplace_back(mVertexBufferIndex++, 0, vk::Format::eR32Sfloat, element.Offset);
				break;
			case BHive::EShaderDataType::Float2:
				mAttributes.emplace_back(mVertexBufferIndex++, 0, vk::Format::eR32G32Sfloat, element.Offset);
				break;
			case BHive::EShaderDataType::Float3:
				mAttributes.emplace_back(mVertexBufferIndex++, 0, vk::Format::eR32G32B32Sfloat, element.Offset);
				break;
			case BHive::EShaderDataType::Float4:
			{
				mAttributes.emplace_back(mVertexBufferIndex++, 0, vk::Format::eR32G32B32A32Sfloat, element.Offset);
				break;
			}
			case BHive::EShaderDataType::Int:
				mAttributes.emplace_back(mVertexBufferIndex++, 0, vk::Format::eR32Sint, element.Offset);
				break;
			case BHive::EShaderDataType::Int2:
				mAttributes.emplace_back(mVertexBufferIndex++, 0, vk::Format::eR32G32Sint, element.Offset);
				break;
			case BHive::EShaderDataType::Int3:
				mAttributes.emplace_back(mVertexBufferIndex++, 0, vk::Format::eR32G32B32Sfloat, element.Offset);
				break;
			case BHive::EShaderDataType::Int4:
				mAttributes.emplace_back(mVertexBufferIndex++, 0, vk::Format::eR32Sfloat, element.Offset);
				break;
			case BHive::EShaderDataType::Bool:
			{
				mAttributes.emplace_back(mVertexBufferIndex++, 0, vk::Format::eR32Sint, element.Offset);
				break;
			}
			case BHive::EShaderDataType::Mat3:
			case BHive::EShaderDataType::Mat4:
			{
				auto count = element.ComponentCount;
				for (uint8_t i = 0; i < count; i++)
				{
					mAttributes.emplace_back(mVertexBufferIndex++, 0, vk::Format::eR32G32B32A32Sfloat, element.Offset + sizeof(float) * count * i);
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

	Ref<VertexBuffer> VertexArray::GetVertexBuffer(uint32_t index) const
	{
		ASSERT(index < mVertexBuffers.size() && index >= 0);
		return mVertexBuffers[index];
	}
} // namespace BHive