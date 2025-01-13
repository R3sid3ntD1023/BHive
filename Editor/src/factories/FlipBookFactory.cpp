#include "FlipBookFactory.h"
#include "sprite/FlipBook.h"

namespace BHive
{
	Ref<Asset> FlipBookFactory::CreateNew()
	{
		auto fb = CreateRef<FlipBook>();
		OnImportCompleted.invoke(fb);
		return fb;
	}

    REFLECT_Factory(FlipBookFactory, FlipBook, ".flipbook");



} // namespace BHive
