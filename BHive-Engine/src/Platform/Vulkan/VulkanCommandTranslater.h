#pragma once

#include "gfx/rendergraph/Pass.h"
#include "VulkanRendererAPI.h"

namespace BHive
{
	struct VulkanCommandTranslator
	{
		static void ExecuteCommandList(const FPhase &phase, FVulkanRendererContext &ctx);

		static void CreateBarriers(const std::vector<FBufferTransition> &transitions, FVulkanRendererContext &ctx);

	private:
		static void BindMaterialSnapshot(const MaterialSnapshot &snap, FVulkanRendererContext &ctx, const FPhase &phase);
	};
} // namespace BHive