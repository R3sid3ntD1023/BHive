#pragma once

#include "gfx/RenderGraph.h"

namespace BHive
{
	class GPUJobSystem
	{
	public:
		std::vector<Scope<FAsyncPass>> Active;

		FAsyncPass *Create() { 
			auto pass = CreateScope<FAsyncPass>();
			auto raw  = pass.get();
			Active.emplace_back(std::move(pass));
			return raw;
		}

		void Update() {
			for (size_t i = 0; i < Active.size();)
			{
				if (Active[i]->IsDone())
				{
					Active[i]->Destroy();
					Active.erase(Active.begin() + 1);
				}
				else
				{
					++i;
				}
			}
		}
	};
}