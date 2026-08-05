#pragma once

#include "gfx/rendergraph/RenderCommandList.h"
#include "VulkanRendererAPI.h"

namespace BHive
{
	class VulkanBindingGroup;

	struct VulkanCommandTranslator
	{
		static void ExecuteCommandList(const FRenderCommandList &list, FVulkanRendererContext &ctx, uint32_t numAttachments);

		static void CreateBarriers(const FRenderCommandList &list, FVulkanRendererContext &ctx);

		static vk::ClearColorValue GetClearColor() { return mClearColor; }

		static vk::ClearDepthStencilValue GetDepthStencilValue() { return mDepthStencilValue; }

	private:
		static void BindMaterialSnapshot(const MaterialSnapshot &snap, FVulkanRendererContext &ctx);

		static void BindMaterialResources(const MaterialSnapshot &snap, VulkanBindingGroup &group);

		static void BindObjectResources(const Ref<GeneralBuffer> &buffer, VulkanBindingGroup &group);

	private:
		static inline vk::ClearColorValue mClearColor{0.f, 0.f, 0.f, 1.0f};
		static inline vk::ClearDepthStencilValue mDepthStencilValue{1.0f, 0};
		static inline std::optional<vk::PrimitiveTopology> mGlobalTopology;
	};
} // namespace BHive