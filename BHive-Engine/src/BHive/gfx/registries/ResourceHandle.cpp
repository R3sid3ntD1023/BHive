#include "ResourceHandle.h"
#include "IResourceRegistry.h"
#include "RegistryManager.h"

namespace BHive
{
	void *ResourceHandle::Resolve() const
	{
		auto reg = ResourceRegistriesManager::GetRegistry(Type);
		if (!reg || !IsValid())
			return nullptr;

		if (!reg->IsValid(*this))
			return nullptr;

		return reg->GetRaw(*this);
	}
} // namespace BHive