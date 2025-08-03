#pragma once

#include "DragDropFactory.h"

namespace BHive
{
	struct DragDropPrefab : public DragDropFactory
	{
		virtual bool CanCreateEntityFrom(const rttr::type &type) override;

		virtual void PostCreateEntity(const Ref<Asset> &asset, Ref<GameObject> &object) override;

		REFLECTABLEV(DragDropFactory)
	};

	REFLECT_EXTERN(DragDropPrefab)
} // namespace BHive