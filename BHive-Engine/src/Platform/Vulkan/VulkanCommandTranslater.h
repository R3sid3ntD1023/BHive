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

	private:
		static void BindMaterialSnapshot(const MaterialSnapshot &snap, FVulkanRendererContext &ctx);

		static void BindMaterialResources(const MaterialSnapshot &snap, VulkanBindingGroup &group);

		static void BindObjectResources(const Ref<GeneralBuffer> &buffer, VulkanBindingGroup &group);
	};
} // namespace BHive