#pragma once

#include "core/Core.h"
#include "gfx/VertexArray.h"
#include "VulkanCore.h"
#include "VulkanUtils.h"

namespace BHive
{
	enum EDrawMode
	{
		Lines = 0x0001,
		Triangles = 0x0004
	};

	enum ClearBitMask : int
	{
		Buffer_Depth = 0x00000100,
		Buffer_Stencil = 0x00000400,
		Buffer_Color = 0x00004000,
	};

	struct MultiDrawIndirectCommand
	{
		uint32_t Count;
		uint32_t InstanceCount;
		uint32_t FirstIndex;
		int32_t BaseVertex;
		uint32_t BaseInstance;
	};

	struct FVulkanFrameData
	{
		vk::raii::CommandBuffer &CommandBuffer;

		vk::Image Image;

		vk::ImageView ImageView;

		uint32_t Frame;
	};


	struct FRenderCommand
	{
		
		FRenderCommand(std::function<void(const FVulkanFrameData &)> &&cmd)
			: mCommand(std::move(cmd))
		{
		}

		void Execute(const FVulkanFrameData &data)
		{
			if (mCommand)
				mCommand(data);
		}

	private:
		std::function<void(const FVulkanFrameData &)> mCommand = nullptr;
	};

	class BHIVE_API RendererAPI
	{

	public:
		RendererAPI() = default;
		virtual ~RendererAPI();

		virtual void Init();
		virtual void Shutdown();

		virtual void ClearColor(float r, float g, float b, float a = 1.0f);
		virtual void Clear(int mask = Buffer_Color | Buffer_Depth | Buffer_Stencil);

		virtual void SetLineWidth(float width);
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h);

		virtual void DrawArrays(EDrawMode mode, const VertexArray &vao, uint32_t count = 0);
		virtual void DrawElements(EDrawMode mode, const VertexArray &vao, uint32_t count = 0);
		virtual void DrawElementsBaseVertex(EDrawMode mode, const VertexArray &vao, uint32_t start, uint32_t start_index, uint32_t count = 0, uint32_t instance_count = 0);
		virtual void DrawElementsRanged(EDrawMode mode, const VertexArray &vao, uint32_t start, uint32_t end, uint32_t count = 0);
		virtual void DrawElementsInstanced(EDrawMode mode, const VertexArray &vao, uint32_t instances, uint32_t count = 0);
		virtual void MultiDrawElementsIndirect(EDrawMode mode, const BufferBase &indirect, const VertexArray &vao, const void *data, size_t drawCount, size_t stride = 0);

		virtual void EnableDepth();
		virtual void DisableDepth();
		virtual void DepthFunc(uint32_t func);

		virtual void CullFront();
		virtual void CullBack();
		virtual void SetCullEnabled(bool enabled);

		virtual void ColorMask(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		virtual void EnableDepthMask(bool mask);
		virtual void EnableBlend(bool enabled);
		virtual void AttachTextureToFramebuffer(uint32_t attachment, uint32_t texture, uint32_t framebuffer);

		virtual void *CreateShader(const uint32_t *data, size_t size);

		virtual void BeginFrame();

		virtual void EndFrame();

		virtual void BindPipeline(const class VulkanPipeline &pipeline);

		virtual void BindDescriptorSets(const vk::raii::PipelineLayout &layout, const std::vector<vk::raii::DescriptorSet> &sets);

		virtual void SubmitCommand(std::function<void(const FVulkanFrameData &)> &&command);

		virtual void SubmitSecondaryCommand(std::function<void(const FVulkanFrameData &)> &&command);

		vk::raii::CommandBuffer &GetCommandBuffer(uint32_t index) { return mCommandBuffers[0].at(index); }

		vk::raii::CommandBuffer &GetCurrentCommandBuffer();

		vk::raii::CommandPool &GetCommandPool() { return mCommandPool; }

		vk::raii::CommandBuffers *AllocateCommandBuffers(uint32_t count);

		const std::vector<vk::raii::CommandBuffers> &GetCommandBuffers() const { return mCommandBuffers; }

	private:
		void CreateCommandPool();

		void CreateCommandBuffers();

	private:
		vk::raii::CommandPool mCommandPool = nullptr;

		vk::ClearColorValue mClearColor{0, 0, 0, 1};

		std::vector<VkShaderModule> mVulkanShaders{};

		std::queue<FRenderCommand> mCommands;

		std::queue<FRenderCommand> mSecondaryCommands;

		std::vector<vk::raii::CommandBuffers> mCommandBuffers;

	};
} // namespace BHive