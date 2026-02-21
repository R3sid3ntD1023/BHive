#pragma once

#include "core/Core.h"
#include "core/events/KeyEvents.h"
#include "VulkanBackend.h"
#include "gfx/RendererAPI.h"

namespace BHive
{
	class VulkanSwapChain;
	class VulkanFramebuffer;
	class VulkanRendererAPI;
	class Window;
	class VulkanWindowContext;

	struct FVulkanFrame
	{
		vk::raii::CommandBuffer& CommandBuffer;

		uint32_t Frame;

		uint32_t ImageIndex;
	};

	using RGExecuteFn = std::function<void(const FVulkanFrame&)>;
	using FRenderCommand = std::function<void(const FVulkanFrame &)>;

	
	enum ECommandType
	{
		ECommandType_PreCommand,
		ECommandType_Command,
		ECommandType_ToScreen
	};


	class BHIVE_API VulkanRendererAPI : public RendererAPI
	{

	public:
		VulkanRendererAPI();

		virtual ~VulkanRendererAPI();

		virtual void Init() override;

		virtual void Shutdown() override;

		virtual void WaitIdle() override;

		virtual void ClearColor(float r, float g, float b, float a = 1.0f) override;

		virtual void Clear(ClearMask mask) override;

		virtual void SetLineWidth(float width) override;

		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h) override;

		virtual void DrawArrays(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t count = 0) override;

		virtual void DrawElements(ETopologyMode mode, const Ref<VertexArray> &vao, uint32_t count = 0) override;

		virtual void DrawElementsBaseVertex(ETopologyMode mode, const VertexArray &vao, uint32_t start, uint32_t start_index, uint32_t count = 0, uint32_t instance_count = 0) override;

		virtual void DrawElementsRanged(ETopologyMode mode, const VertexArray &vao, uint32_t start, uint32_t end, uint32_t count = 0) override;

		virtual void DrawElementsInstanced(ETopologyMode mode, const VertexArray &vao, uint32_t instances, uint32_t count = 0) override;

		virtual void MultiDrawElementsIndirect(ETopologyMode mode, const BufferBase &indirect, const VertexArray &vao, size_t drawCount, size_t stride = 0) override;

		virtual void Dispath(uint32_t x, uint32_t y, uint32_t z) override;

		virtual void ColorMask(uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;

		vk::Result RenderFrame(VulkanWindowContext* ctx);

		void SubmitCommand(const FRenderCommand &command, ECommandType type = ECommandType_Command);

		vk::raii::DescriptorPool &GetDescriptorPool() { return mDescriptorPool; }

		virtual EAPI GetAPI() const override { return EAPI::Vulkan; }

		void QueueDeletion(std::function<void(uint32_t)> fn);

	private:
		void ProcessDeletionQueue(uint32_t frame);

	public:
		void SetCurrentContext(VulkanWindowContext *ctx);

		VulkanWindowContext *GetCurrentContext() const { return mCurrentContext; }

	private:
		vk::raii::Device &mDevice;

		vk::raii::DescriptorPool mDescriptorPool = nullptr;

		vk::ClearColorValue mClearColor{0, 0, 0, 1};

		std::unordered_map<ECommandType, std::queue<FRenderCommand>> mCommands;

		std::atomic<bool> mDeviceRecreationInProgress{false};

		VulkanWindowContext *mCurrentContext = nullptr;

		uint32_t  mCompletedFrame = 0;

		friend class VulkanFramebuffer;
	};
} // namespace BHive