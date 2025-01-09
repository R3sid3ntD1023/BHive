#include "InputContextFactory.h"
#include "input/InputContext.h"

namespace BHive
{
	Ref<Asset> InputContextFactory::CreateNew()
	{
		auto ic =  CreateRef<InputContext>();
		OnImportCompleted.invoke(ic);
		return ic;
	}

    REFLECT_Factory(InputContextFactory, InputContext, ".input")



} // namespace BHive
