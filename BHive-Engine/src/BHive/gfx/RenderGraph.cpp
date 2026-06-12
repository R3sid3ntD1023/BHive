#include "RenderGraph.h"
#include "gfx/material/Material.h"

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

	void FRenderCommandList::Push(const std::string &name, RenderCommand cmd)
	{
		mCommands.push_back({name, std::move(cmd)});
	}

	void FRenderCommandList::Execute(IRendererContext &ctx) const
	{
		for (auto &cmd : mCommands)
		{
			cmd.Func(ctx);
		}
	}

	FRenderGraphPass &RenderGraph::AddPass(const std::string &name, EPassType type)
	{
		auto &pass = mPasses.emplace_back();
		pass.Name = name;
		pass.Type = type;
		return pass;
	}

	void RenderGraph::Append(const RenderGraph &graph)
	{
		auto &passes = graph.GetPasses();
		mPasses.insert(mPasses.end(), passes.begin(), passes.end());
	}

	bool RenderGraph::Empty() const
	{
		return mPasses.empty();
	}

	const std::vector<FRenderGraphPass> &RenderGraph::GetPasses() const
	{
		return mPasses;
	}

} // namespace BHive