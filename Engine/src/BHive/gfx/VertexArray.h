#pragma once

#include "Buffers.h"
#include "VulkanCore.h"

namespace BHive
{
	class BHIVE_API VertexArray
	{
	public:
		VertexArray();
		virtual ~VertexArray();

		virtual void Bind() const;
		virtual void UnBind() const;

		virtual void SetIndexBuffer(const Ref<IndexBuffer> &indexbuffer);
		virtual void AddVertexBuffer(const Ref<VertexBuffer> &vertexbuffer);

		virtual const Ref<IndexBuffer> &GetIndexBuffer() const { return mIndexBuffer; }
		virtual void BindBuffersBase(uint32_t binding) const;

		virtual Ref<VertexBuffer> GetVertexBuffer(uint32_t index) const;
		virtual std::vector<Ref<VertexBuffer>> GetVertexBuffers() const { return mVertexBuffers; }

		const vk::VertexInputBindingDescription &GetBindingDescription() const { return mBinding; }

		const std::vector<vk::VertexInputAttributeDescription> &GetAttributeDescriptions() const { return mAttributes; }

	private:
		vk::VertexInputBindingDescription mBinding;
		std::vector<vk::VertexInputAttributeDescription> mAttributes;

		Ref<IndexBuffer> mIndexBuffer;
		std::vector<Ref<VertexBuffer>> mVertexBuffers;

		uint32_t mVertexBufferIndex{0};
	};
} // namespace BHive