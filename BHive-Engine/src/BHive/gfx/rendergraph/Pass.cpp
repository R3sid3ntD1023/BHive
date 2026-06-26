#include "Pass.h"

namespace BHive
{
	
	void FPass::BeginPhase()
	{
		Phases.emplace_back();
		mCurrentPhase = (int32_t)Phases.size() - 1;
	}

	void FPass::BeginPhase(const std::string &name)
	{
		Phases.emplace_back(name);
		mCurrentPhase = (int32_t)Phases.size() - 1;
	}

	void FPass::Push(Ref<Texture> tex, EImageAccess access, ImageSubresourceRange range)
	{
		Phases[mCurrentPhase].ImageUsages.emplace_back(tex, access, range);
	}

	void FPass::EndPhase()
	{
		mCurrentPhase = -1;
	}
}