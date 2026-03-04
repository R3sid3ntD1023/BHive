#include "ISetManager.h"

namespace BHive
{
	void GlobalSetSystem::Register(uint64_t hash, Ref<ISetManager> &manager)
	{
		mGlobalManagers[hash] = manager;
	}

	ISetManager *GlobalSetSystem::Get(uint64_t hash) const
	{
		return mGlobalManagers.at(hash).get();
	}

} // namespace BHive