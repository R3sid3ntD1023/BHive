#pragma once

#include "core/Core.h"
#include "core/events/KeyEvents.h"
#include "gfx/RendererAPI.h"
#include "gfx/VertexArray.h"
#include "VulkanCore.h"
#include "VulkanUtils.h"

namespace BHive
{
	struct FVulkanFrameData
	{
		vk::raii::CommandBuffer &CommandBuffer;

		vk::Image Image;

		vk::ImageView ImageView;

		uint32_t Frame;
	};

	typedef std::function<void(const FVulkanFrameData &)> FRenderCommand;

	class BHIVE_API VulkanRendererAPI : public RendererAPI
	{

	public:
		VulkanRendererAPI();

		virtual ~VulkanRendererAPI();

		virtual void Init() override;

		virtual void Shutdown() override;

		virtual void ClearColor(float r, float g, float b, float a = 1.0f) override;

		virtual void Clear(int mask = Buffer_Color | Buffer_Depth | Buffer_Stencil) override;

		virtual void SetLineWidth(float width) override;

		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h) override;

		virtual void DrawArrays(EDrawMode mode, const VertexArray &vao, uint32_t count = 0) override;

		virtual void DrawElements(EDrawMode mode, const VertexArray &vao, uint32_t count = 0) override;

		virtual void DrawElementsBaseVertex(EDrawMode mode, const VertexArray &vao, uint32_t start, uint32_t start_index, uint32_t count = 0, uint32_t instance_count = 0) override;

		virtual void DrawElementsRanged(EDrawMode mode, const VertexArray &vao, uint32_t start, uint32_t end, uint32_t count = 0) override;

		virtual void DrawElementsInstanced(EDrawMode mode, const VertexArray &vao, uint32_t instances, uint32_t count = 0) override;

		virtual void MultiDrawElementsIndirect(EDrawMode mode, const BufferBase &indirect, const VertexArray &vao, const void *data, size_t drawCount, size_t stride = 0) override;

		virtual void EnableDepth() override;

		virtual void DisableDepth() override;

		virtual void DepthFunc(uint32_t func) override;

		virtual void CullFront() override;

		virtual void CullBack() override;

		virtual void SetCullEnabled(bool enabled) override;

		virtual void ColorMask(uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;

		virtual void EnableDepthMask(bool mask) override;

		virtual void EnableBlend(bool enabled) override;

		virtual void AttachTextureToFramebuffer(uint32_t attachment, uint32_t texture, uint32_t framebuffer) override;

		void BeginFrame();

		void EndFrame();

		void SubmitCommand(const FRenderCommand &command);

		void SubmitSecondaryCommand(const FRenderCommand &command);

		vk::raii::CommandBuffer &GetCommandBuffer(uint32_t index) { return mCommandBuffers[0].at(index); }

		vk::raii::CommandBuffer &GetCurrentCommandBuffer();

		vk::raii::CommandPool &GetCommandPool() { return mCommandPool; }

		vk::raii::CommandBuffers *AllocateCommandBuffers(uint32_t count);

		const std::vector<vk::raii::CommandBuffers> &GetCommandBuffers() const { return mCommandBuffers; }

		virtual EAPI GetAPI() const override { return EAPI::Vulkan; }

	private:
		void CreateCommandPool();

		void CreateCommandBuffers();

	private:
		vk::raii::Device &mDevice;

		vk::raii::CommandPool mCommandPool = nullptr;

		std::vector<vk::raii::CommandBuffers> mCommandBuffers;

		vk::ClearColorValue mClearColor{0, 0, 0, 1};

		std::queue<FRenderCommand> mCommands;

		std::queue<FRenderCommand> mSecondaryCommands;

		std::atomic<bool> mDeviceRecreationInProgress{false};
	};
} // namespace BHive