#include "gfx/RenderCommand.h"
#include "VulkanBuffers.h"
#include "VulkanRendererAPI.h"
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
			return vk::Format::eR32G32B32A32Sfloat;
		}
	} // namespace utils

	VulkanVertexArray::VulkanVertexArray(const std::vector<Ref<VertexBuffer>> &vertex_buffers, const Ref<IndexBuffer> &index_buffer)
		: mVertexBuffers(vertex_buffers),
		  mIndexBuffer(index_buffer)
	{
		for (auto &vb : vertex_buffers)
		{
			CreateBindingsAndAttributes(vb);
		}
	}

	void VulkanVertexArray::Bind() const
	{
		auto bindings = mBindings;
		auto attributes = mAttributes;
		auto vertex_buffers = mVertexBuffers;
		auto index_buffer_ref = mIndexBuffer;

		auto cmd = [=](const FVulkanFrameData &data)
		{
			auto size = vertex_buffers.size();
			std::vector<vk::Buffer> vk_vertex_buffers(size);
			std::vector<vk::DeviceSize> offsets(size);
			vk::Buffer index_buffer;

			if (index_buffer_ref)
				index_buffer = *std::dynamic_pointer_cast<VulkanIndexBuffer>(index_buffer_ref);

			for (uint32_t i = 0; i < size; i++)
			{
				auto &vb = vertex_buffers[i];
				vk_vertex_buffers[i] = (*std::dynamic_pointer_cast<VulkanVertexBuffer>(vb));
				offsets[i] = 0;
			}

			data.CommandBuffer.setVertexInputEXT(bindings, attributes);
			data.CommandBuffer.bindVertexBuffers(0, vk_vertex_buffers, offsets);

			if (index_buffer)
			{
				data.CommandBuffer.bindIndexBuffer(index_buffer, 0, vk::IndexType::eUint32);
			}
		};

		RenderCommand::GetAPI<VulkanRendererAPI>()->SubmitCommand(cmd);
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
		mVertexBuffers.push_back(vertexbuffer);
		CreateBindingsAndAttributes(vertexbuffer);
	}

	void VulkanVertexArray::CreateBindingsAndAttributes(const Ref<VertexBuffer> &vertexbuffer)
	{
		auto &layout = vertexbuffer->GetLayout();
		auto &elements = layout.GetElements();
		auto stride = layout.GetStride();

		if (elements.size() == 0)
			return;

		uint32_t binding = mBindings.size();

		mBindings.emplace_back(vk::VertexInputBindingDescription2EXT(binding, stride, vk::VertexInputRate::eVertex, 0));

		{
			const auto &b = mBindings.back();
			LOG_INFO("VulkanVertexArray: Created binding index={} stride={} inputRate={} divisor={}", b.binding, b.stride, (uint32_t)b.inputRate, b.divisor);
		}

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
			
				mAttributes.emplace_back(mVertexAttributeIndex++, binding, utils::GetVulkanFormat(type), (uint32_t)element.Offset);
				{
					const auto &a = mAttributes.back();
					LOG_INFO("VulkanVertexArray: Attr loc={} binding={} format={} offset={} (elem type={} size={})", a.location, a.binding, (int)a.format, a.offset, (int)type, element.Size);
				}
				break;
			}
			case BHive::EShaderDataType::Mat3:
			case BHive::EShaderDataType::Mat4:
			{
				auto count = element.ComponentCount;
				for (uint8_t i = 0; i < count; i++)
				{
					// For matrices we create one attribute per column (or row depending on layout),
					// offset each attribute by the size of a column (count * sizeof(float)).
					mAttributes.emplace_back(mVertexAttributeIndex++, binding, utils::GetVulkanFormat(type), (uint32_t)(element.Offset + 16 * i));

					{
						const auto &a = mAttributes.back();
						LOG_INFO(
							"VulkanVertexArray: Mat Attr loc={} binding={} format={} offset={} (mat col {} of {}, elem type={} size={})", a.location, a.binding, (int)a.format, a.offset, (int)i,
							(int)count, (int)type, element.Size);
					}
				}
				break;
			}
			default:
				break;
			}
		}
	}

} // namespace BHive