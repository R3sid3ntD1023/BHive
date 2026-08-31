#include "RenderGraph.h"
#include "gfx/Texture.h"

namespace BHive
{
	void RenderGraph::Append(const RenderGraph &graph)
	{
		auto &passes = graph.GetPasses();
		mPasses.insert(mPasses.end(), std::make_move_iterator(passes.begin()), std::make_move_iterator(passes.end()));
	}

	bool RenderGraph::Empty() const
	{
		return mPasses.empty();
	}

	FPass &RenderGraph::AddPass(const std::string &name, EPassType type, FPassState state)
	{
		auto &pass = mPasses.emplace_back();
		pass.Name = name;
		pass.Type = type;
		pass.State = state;
		return pass;
	}

	const std::vector<FPass> &RenderGraph::GetPasses() const
	{
		return mPasses;
	}

	std::vector<FPass> &RenderGraph::GetPasses()
	{
		return mPasses;
	}

	void RenderGraph::DebugPrint()
	{
		for (auto &pass : mPasses)
		{
			LOG_TRACE("Pass : {}", pass.Name);

			for (auto &phase : pass.Phases)
			{
				LOG_TRACE("\t Phase: {}", phase.Name);

				for (auto &tex : phase.Images)
				{
					tex.Texture.As<Texture>()->DebugPrintState();

					LOG_TRACE("\t\tTransition -> [{}:{}]", to_string(tex.Access), to_string(tex.Range));
				}

				auto it = phase.Commands.begin();
				auto end = phase.Commands.end();

				while (it != end)
				{
					auto &header = (*it).first;
					LOG_TRACE("\t\t{}", (int)header.Type);
					++it;
				}
			}
		}
	}

} // namespace BHive