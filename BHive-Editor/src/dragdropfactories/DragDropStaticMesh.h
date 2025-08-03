#pragma once

#include "DragDropFactory.h"

namespace BHive
{
	class DragDropStaticMesh : public DragDropFactory
	{
	public:
		virtual bool CanCreateEntityFrom(const rttr::type &type) override;

		virtual void PostCreateEntity(const Ref<Asset> &asset, Ref<GameObject> &entity) override;

		REFLECTABLEV(DragDropFactory)
	};

	REFLECT_EXTERN(DragDropStaticMesh)
} // namespace BHive