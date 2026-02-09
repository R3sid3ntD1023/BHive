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
		vk::raii::CommandBuffer& CommandBuffer;

		uint32_t Frame;

	};

	typedef std::function<void(const FVulkanFrameData &)> FRenderCommand;

	enum ECommandType
	{
		ECommandType_PreCommand,
		ECommandType_Command
	};

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

		virtual void DrawArrays(EDrawMode mode, const Ref<VertexArray> &vao, uint32_t count = 0) override;

		virtual void DrawElements(EDrawMode mode, const Ref<VertexArray> &vao, uint32_t count = 0) override;

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

		vk::raii::CommandBuffer& RenderFrame(vk::ImageLayout& layout, vk::Image &image, vk::raii::ImageView &image_view, const vk::Extent2D& extent);

		void SubmitCommand(const FRenderCommand &command, ECommandType type = ECommandType_Command);

		vk::raii::CommandPool &GetCommandPool() { return mCommandPool; }

		vk::raii::DescriptorPool &GetDescriptorPool() { return mDescriptorPool; }

		virtual EAPI GetAPI() const override { return EAPI::Vulkan; }

		uint32_t GetCurrentFrame() const { return mCurrentFrame; }

		void AdvanceFrame();

	private:
		vk::raii::Device &mDevice;

		vk::raii::CommandPool mCommandPool = nullptr;

		vk::raii::CommandBuffers mCommandBuffers = nullptr;

		vk::raii::DescriptorPool mDescriptorPool = nullptr;

		vk::ClearColorValue mClearColor{0, 0, 0, 1};

		std::unordered_map<ECommandType, std::queue<FRenderCommand>> mCommands;

		std::atomic<bool> mDeviceRecreationInProgress{false};

		uint32_t mCurrentFrame = 0;
	};
} // namespace BHive