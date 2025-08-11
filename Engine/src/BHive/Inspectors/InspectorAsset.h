#pragma once

#include "Inspector.h"

namespace BHive
{
	template <typename T>
	struct Inspector_Asset : public Inspector
	{
		INSPECTOR_BODY()

		REFLECTABLEV(Inspector)
	};

} // namespace BHive

#include "InspectorAsset.inl"

#define REFLECT_ASSET_INSPECTOR(cls) REFLECT_INSPECTOR(Inspector_Asset<cls>, Ref<cls>)
