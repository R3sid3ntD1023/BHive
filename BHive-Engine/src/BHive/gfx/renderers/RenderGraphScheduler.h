#pragma once

#include "gfx/rendergraph/Graph.h"

namespace BHive
{
	class Graph;

	struct PassConfig
	{
		std::string DefaultPassName = "Default Pass";
		EPassType DefaultPassType = EPassType::Present;
		bool DebugMarkers = false;
	};

	class RenderGraphScheduler
	{
	public:
		void BeginFrame(Graph &graph, const PassConfig &config);
		void Finalize();

		FPass &GetActivePass();
		FPass &BeginPass(const std::string &name, EPassType type, const FPassState &state = {});
		void EndPass();

		void DeferPass(const std::string &name, EPassType type, std::function<void(FPass &)> fn);

	private:
		void DebugPass(const std::string &msg);

		Graph *mGraph = nullptr;
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
} // namespace BHive