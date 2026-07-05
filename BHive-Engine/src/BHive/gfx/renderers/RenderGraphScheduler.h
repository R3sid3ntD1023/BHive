#pragma once

#include "gfx/RenderGraph.h"


namespace BHive
{
	struct PassConfig
	{
		std::string DefaultPassName = "Default Pass";
		EPassType DefaultPassType = EPassType::Present;
		bool DebugMarkers = false;
	};

	

	class RenderGraphScheduler
	{
	public:
		void BeginFrame(RenderGraph &graph, const PassConfig& config);
		void Finalize(RenderGraph &graph);

		FPass &GetActivePass();
		FPass &BeginPass(const std::string &name, EPassType type, const FPassState &state = {});
		void EndPass();

		void DeferPass(const std::string &name, EPassType type, std::function<void(FPass &)> fn);

	private:
		void DebugPass(const std::string &msg);

		RenderGraph *mGraph = nullptr;
		const PassConfig *mConfig = nullptr;
		FPass *mActivePass = nullptr;
		
		struct FDeferred
		{
			std::string Name;
			EPassType Type;
			std::function<void(FPass &)> Fn;
		};

		std::vector<FDeferred> mDeferred;
	};
}