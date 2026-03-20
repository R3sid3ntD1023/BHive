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
			case BHive::EShaderDataType::UInt:
				return vk::Format::eR32Uint;
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

	VulkanVertexArray::VulkanVertexArray()
		: mVertexAttributeIndex(0)
	{
	}

	VulkanVertexArray::VulkanVertexArray(const std::vector<Ref<VertexBuffer>> &vertex_buffers, const Ref<IndexBuffer> &index_buffer)
		: mVertexAttributeIndex(0),
		  mIndexBuffer(index_buffer),
		  mVertexBuffers(vertex_buffers)
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
		auto index_buffer = mIndexBuffer;
		auto vertex_buffers = mVertexBuffers;

		auto &pass = RenderCommand::GetActivePass();
		pass.CommandList.Push(
			"Bind vertexArray",
			[=](IRendererContext &ctx)
			{
				auto &vk_ctx = CastRef<FVulkanRendererContext>(ctx);
				const auto current_frame = vk_ctx.Frame;

				
				auto vb_count = vertex_buffers.size();
				std::vector<vk::Buffer> vertex_handles(vb_count, VK_NULL_HANDLE);
				for (size_t i = 0; i < vb_count; i++)
				{
					vertex_handles[i] = vertex_buffers[i]->GetNativeHandle(current_frame).As<AllocatedBuffer>()->Buffer;
				}

				std::vector<vk::DeviceSize> offsets(vb_count, 0);

				ASSERT(bindings.size() && attributes.size());

				vk_ctx.CommandBuffer.setVertexInputEXT(bindings, attributes);
				vk_ctx.CommandBuffer.bindVertexBuffers(0, vertex_handles, offsets);

				if (index_buffer)
				{
					auto index_handle = index_buffer->GetNativeHandle(current_frame).As<AllocatedBuffer>()->Buffer;
					vk_ctx.CommandBuffer.bindIndexBuffer(index_handle, 0, vk::IndexType::eUint32);
				}
			});
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

		if (elements.size() == 0)
			return;

		uint32_t binding = mBindings.size();

		mBindings.emplace_back(vk::VertexInputBindingDescription2EXT(binding, stride, vk::VertexInputRate::eVertex));

		for (const auto &element : elements)
		{
			auto type = element.Type;
			switch (type)
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
				mAttributes.emplace_back(mVertexAttributeIndex++, binding, utils::GetVulkanFormat(type), (uint32_t)element.Offset);
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
					mAttributes.emplace_back(mVertexAttributeIndex++, binding, utils::GetVulkanFormat(type), (uint32_t)(element.Offset + count * sizeof(float) * i));
				}
				break;
			}
			default:
				break;
			}
		}
	}

} // namespace BHive