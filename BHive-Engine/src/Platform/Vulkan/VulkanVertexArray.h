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

		VulkanVertexArray(const std::vector<Ref<VertexBuffer>> &vertex_buffers, const Ref<IndexBuffer> &index_buffer = nullptr);

		virtual void Bind() const override;

		virtual void UnBind() const override;

		virtual void SetIndexBuffer(const Ref<IndexBuffer> &indexbuffer) override;

		virtual void AddVertexBuffer(const Ref<VertexBuffer> &vertexbuffer) override;

		virtual const Ref<IndexBuffer> &GetIndexBuffer() const override { return mIndexBuffer; }

		virtual const std::vector<Ref<VertexBuffer>> &GetVertexBuffers() const override { return mVertexBuffers; }

	private:
		void CreateBindingsAndAttributes(const Ref<VertexBuffer> &vertexbuffer);

		
	private:
		Ref<IndexBuffer> mIndexBuffer;

		std::vector<Ref<VertexBuffer>> mVertexBuffers;

		Bindings mBindings;

		Attributes mAttributes;

		uint32_t mVertexAttributeIndex = 0;

		struct PerFrameBindings
		{
			vk::Buffer IndexBuffer;
			std::vector<vk::Buffer> VertexBuffers;
		};

		std::array<PerFrameBindings, MAX_FRAMES_IN_FLIGHT> mCachedHandles;
	};
} // namespace BHive