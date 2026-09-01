#pragma once

#include "gfx/VertexArray.h"
#include "VulkanBackend.h"

namespace BHive
{

	class BHIVE_API VulkanVertexArray : public VertexArray
	{
	public:
		using Bindings = std::vector<vk::VertexInputBindingDescription2EXT>;
		using Attributes = std::vector<vk::VertexInputAttributeDescription2EXT>;

	public:
		VulkanVertexArray();

		VulkanVertexArray(const std::vector<VertexBufferPtr> &vertex_buffers, IndexBufferPtr index_buffer = {});

		void Bind(vk::CommandBuffer cmd, uint32_t frame);

		virtual void SetIndexBuffer(IndexBufferPtr indexbuffer) override;

		virtual void AddVertexBuffer(VertexBufferPtr vertexbuffer) override;

		virtual IndexBufferPtr GetIndexBuffer() const override { return mIndexBuffer; }

		virtual const std::vector<VertexBufferPtr> &GetVertexBuffers() const override { return mVertexBuffers; }

	private:
		void CreateBindingsAndAttributes(VertexBufferPtr vertexbuffer);

		private:
		IndexBufferPtr mIndexBuffer;

		std::vector<VertexBufferPtr> mVertexBuffers;

		Bindings mBindings;

		Attributes mAttributes;

		uint32_t mVertexAttributeIndex = 0;
	};
} // namespace BHive