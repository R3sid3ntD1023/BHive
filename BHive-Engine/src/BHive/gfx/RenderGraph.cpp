#include "RenderGraph.h"
#include "gfx/Texture.h"

namespace BHive
{
	void FResourceUpdateList::Push(UpdateCommand cmd)
	{
		mUpdateCommands.push_back(std::move(cmd));
	}

	void FResourceUpdateList::Append(FResourceUpdateList &updates)
	{
		mUpdateCommands.insert(mUpdateCommands.end(), updates.mUpdateCommands.begin(), updates.mUpdateCommands.end());
	}

	void FResourceUpdateList::Execute(IRendererContext &ctx) const
	{
		for (auto &cmd : mUpdateCommands)
		{
			cmd(ctx);
		}
	}

	void FResourceUpdateList::Clear()
	{
		mUpdateCommands.clear();
	}

	bool FResourceUpdateList::Empty() const
	{
		return mUpdateCommands.empty();
	}


	void RenderGraph::Append(const RenderGraph &graph)
	{
		auto &passes = graph.GetPasses();
		mPasses.insert(mPasses.end(), std::make_move_iterator(passes.begin()), std::make_move_iterator(passes.end()));
	}

	bool RenderGraph::Empty() const
	{
		return mPasses.empty();
	}

	FPass &RenderGraph::AddPass(const std::string &name, EPassType type)
	{
		auto &pass = mPasses.emplace_back();
		pass.Name = name;
		pass.Type = type;
		return pass;
	}

	const std::vector<FPass> &RenderGraph::GetPasses() const
	{
		return mPasses;
	}

	void RenderGraph::DebugPrint()
	{
		for (auto& pass : mPasses)
		{
			LOG_TRACE("Pass : {}", pass.Name);

			for (auto& phase : pass.Phases)
			{
				LOG_TRACE("\t Phase: {}", phase.Name);

				for (auto& tex : phase.ImageUsages)
				{
					tex.Texture->DebugPrintState();

					LOG_TRACE("\t\tTransition -> [{}:{}]", to_string(tex.Access), to_string(tex.Range));
				}

				for (auto &cmd : phase.CommandList.GetCommands())
				{
					LOG_TRACE("\t\t{}", cmd.Name)
				}
			}
		}
	}


} // namespace BHive