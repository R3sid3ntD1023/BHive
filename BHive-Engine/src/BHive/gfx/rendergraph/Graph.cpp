#include "Graph.h"

namespace BHive
{
	void Graph::Append(Graph &graph)
	{
		auto &passes = graph.GetPasses();
		mPasses.insert(mPasses.end(), passes.begin(), passes.end());
		graph.mPasses.clear();
	}

	FPass &Graph::AddPass(const std::string &name, EPassType type, FPassState state)
	{
		auto &pass = mPasses.emplace_back();
		pass.Name = name;
		pass.Type = type;
		pass.State = state;
		return pass;
	}

	void Graph::DebugPrint()
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