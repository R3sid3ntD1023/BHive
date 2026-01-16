#include "VulkanBuffers.h"
#include "VulkanVertexArray.h"

namespace BHive
{
	namespace utils
	{
		vk::Format GetVulkanFormat(EShaderDataType type)
		{
			switch (type)
			{
			case BHive::EShaderDataType::Float:
				return vk::Format::eR32Sfloat;
			case BHive::EShaderDataType::Float2:
				return vk::Format::eR32G32Sfloat;
			case BHive::EShaderDataType::Float3:
				return vk::Format::eR32G32B32Sfloat;
			case BHive::EShaderDataType::Float4:
				return vk::Format::eR32G32B32A32Sfloat;
			case BHive::EShaderDataType::Int:
				return vk::Format::eR32Sint;
			case BHive::EShaderDataType::Int2:
				return vk::Format::eR32G32Sint;
			case BHive::EShaderDataType::Int3:
				return vk::Format::eR32G32B32Sint;
			case BHive::EShaderDataType::Int4:
				return vk::Format::eR32G32B32A32Sint;
			case BHive::EShaderDataType::Bool:
				return vk::Format::eR32Sint;
			case BHive::EShaderDataType::Mat3:
				return vk::Format::eR32G32B32Sfloat;
			case BHive::EShaderDataType::Mat4:
				return vk::Format::eR32G32B32A32Sfloat;
			}

			ASSERT(false);
		}
	} // namespace utils

	VulkanVertexArray::VulkanVertexArray()
		: mDevice(VulkanCore::GetLogicalDevice())
	{
	}

	VulkanVertexArray::VulkanVertexArray(const std::initializer_list<Ref<VertexBuffer>> vertex_buffers, const Ref<IndexBuffer> &index_buffer)
		: mDevice(VulkanCore::GetLogicalDevice()),
		  mVertexBuffers(vertex_buffers),
		  mIndexBuffer(index_buffer)
	{
		for (auto &vb : vertex_buffers)
		{
			CreateBindingsAndAttributes(vb);
		}
	}

	void VulkanVertexArray::Bind() const
	{
	}

	void VulkanVertexArray::UnBind() const
	{
	}

	void VulkanVertexArray::SetIndexBuffer(const Ref<IndexBuffer> &indexbuffer)
	{
		mIndexBuffer = indexbuffer;
	}

	void VulkanVertexArray::AddVertexBuffer(const Ref<VertexBuffer> &vertexbuffer)
	{
		mVertexBuffers.emplace_back(vertexbuffer);
		CreateBindingsAndAttributes(vertexbuffer);
	}

	void VulkanVertexArray::CreateBindingsAndAttributes(const Ref<VertexBuffer> &vertexbuffer)
	{
		auto &layout = vertexbuffer->GetLayout();
		auto &elements = layout.GetElements();
		auto stride = layout.GetStride();
		auto index = (uint32_t)mVertexBuffers.size();

		if (elements.size() == 0)
			return;

		mBinding = vk::VertexInputBindingDescription2EXT(0, stride, vk::VertexInputRate::eVertex, 1);

		for (const auto &element : elements)
		{
			auto type = element.Type;
			switch (type)
			{
			case BHive::EShaderDataType::Float:
			case BHive::EShaderDataType::Float2:
			case BHive::EShaderDataType::Float3:
			case BHive::EShaderDataType::Float4:
			case BHive::EShaderDataType::Int:
			case BHive::EShaderDataType::Int2:
			case BHive::EShaderDataType::Int3:
			case BHive::EShaderDataType::Int4:
			case BHive::EShaderDataType::Bool:
			{
				mAttributes.emplace_back(index++, 0, utils::GetVulkanFormat(type), (uint32_t)element.Offset);
				break;
			}
			case BHive::EShaderDataType::Mat3:
			case BHive::EShaderDataType::Mat4:
			{
				auto count = element.ComponentCount;
				for (uint8_t i = 0; i < count; i++)
				{
					mAttributes.emplace_back(index++, 0, utils::GetVulkanFormat(type), (uint32_t)(element.Offset + sizeof(float) * count * i));
				}
				break;
			}
			default:
				break;
			}
		}
	}

} // namespace BHive