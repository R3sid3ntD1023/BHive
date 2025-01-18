#include "FlipBookFactory.h"
#include "sprite/FlipBook.h"

namespace BHive
{
	Ref<Asset> FlipBookFactory::CreateNew()
	{
		auto fb = CreateRef<FlipBook>();
		OnAssetCreated.invoke(fb);
		return fb;
	}

	REFLECT_FACTORY(FlipBookFactory, FlipBook, ".flipbook");

} // namespace BHive
