#include "RenderGraphScheduler.h"

namespace BHive
{
	void RenderGraphScheduler::BeginFrame(RenderGraph &graph, const PassConfig &config)
	{
		mGraph = &graph;
		mConfig = &config;
		mActivePass = nullptr;
	}

	void RenderGraphScheduler::Finalize(RenderGraph &graph)
	{
		for (auto& d : mDeferred)
		{
			auto &pass = graph.AddPass(d.Name, d.Type);
			d.Fn(pass);
		}

		mDeferred.clear();
	}

	FPass &RenderGraphScheduler::GetActivePass()
	{
		if (!mActivePass)
		{
			mActivePass = &mGraph->AddPass(mConfig->DefaultPassName, mConfig->DefaultPassType);
		}

		return *mActivePass;
	}

	FPass &RenderGraphScheduler::BeginPass(const std::string &name, EPassType type)
	{
		DebugPass("BeginPass: " + name);
		mActivePass = &mGraph->AddPass(name, type);
		return *mActivePass;
	}

	void RenderGraphScheduler::EndPass()
	{
		if (mActivePass)
			DebugPass("Endpass: " + mActivePass->Name);

		mActivePass = nullptr;
	}

	void RenderGraphScheduler::DeferPass(const std::string &name, EPassType type, std::function<void(FPass &)> fn)
	{
		mDeferred.push_back({name, type, fn});
	}

	void RenderGraphScheduler::DebugPass(const std::string &msg)
	{
		if (mConfig && mConfig->DebugMarkers)
			LOG_TRACE("[RenderGraph] {}", msg);
	}
} // namespace BHive