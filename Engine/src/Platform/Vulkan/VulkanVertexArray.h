#pragma once

#include "gfx/VertexArray.h"
#include "gfx/VulkanCore.h"

namespace BHive
{
	class BHIVE_API VulkanVertexArray : public VertexArray
	{
	public:
		using Bindings = vk::VertexInputBindingDescription2EXT;
		using Attributes = std::vector<vk::VertexInputAttributeDescription2EXT>;

	public:
		VulkanVertexArray();

		VulkanVertexArray(const std::initializer_list<Ref<VertexBuffer>> vertex_buffers, const Ref<IndexBuffer> &index_buffer = nullptr);

		virtual void Bind() const override;

		virtual void UnBind() const override;

		virtual void SetIndexBuffer(const Ref<IndexBuffer> &indexbuffer) override;

		virtual void AddVertexBuffer(const Ref<VertexBuffer> &vertexbuffer) override;

		virtual const Ref<IndexBuffer> &GetIndexBuffer() const override { return mIndexBuffer; }

		virtual const std::vector<Ref<VertexBuffer>> &GetVertexBuffers() const override { return mVertexBuffers; }

		const Bindings &GetBindingDescription() const { return mBinding; }

		const Attributes &GetAttributeDescriptions() const { return mAttributes; }

	private:
		void CreateBindingsAndAttributes(const Ref<VertexBuffer> &vertexbuffer);

	private:
		vk::raii::Device &mDevice;

		Ref<IndexBuffer> mIndexBuffer;

		std::vector<Ref<VertexBuffer>> mVertexBuffers;

		Bindings mBinding;

		Attributes mAttributes;

		uint32_t mVertexAttributeIndex{0};
	};
} // namespace BHive