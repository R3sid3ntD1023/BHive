#include "input/InputContext.h"
#include "InputContextFactory.h"

namespace BHive
{
	Ref<Asset> InputContextFactory::CreateNew()
	{
		auto ic = CreateRef<InputContext>();
		OnAssetCreated.invoke(ic);
		return ic;
	}

	REFLECT_FACTORY(InputContextFactory)

} // namespace BHive
