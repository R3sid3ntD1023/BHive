#pragma once

#include "gfx/rendergraph/Pass.h"
#include "VulkanRendererAPI.h"

namespace BHive
{
	class VulkanBindingGroup;

	struct VulkanCommandTranslator
	{
		static void ExecuteCommandList(const FPass &pass, const FPhase &phase, FVulkanRendererContext &ctx);

		static void CreateBarriers(const FRenderCommandList &list, FVulkanRendererContext &ctx);

	private:
		static void BindGlobals(VulkanShader *shader, const FPass &pass);

		static void BindMaterialSnapshot(const MaterialSnapshot &snap, FVulkanRendererContext &ctx, const FPass &phase);

		static void BindMaterialResources(const MaterialSnapshot &snap, VulkanBindingGroup &group);
	};
} // namespace BHive