#include "RenderCommandList.h"

namespace BHive
{
	void FRenderCommandList::PushInternal(const std::string &name, FCommand &&cmd)
	{
		FEntry e;
		e.Name = name;
		e.Cmd = std::move(cmd);
		mCommands.emplace_back(std::move(e));
	}

	void FRenderCommandList::Execute(IRendererContext &ctx) const
	{
		for (auto &cmd : mCommands)
		{
			cmd.Cmd.Fn(ctx);
		}
	}

}