#pragma once

#include "DragDropFactory.h"

namespace BHive
{
	class DragDropAudio : public DragDropFactory
	{
	public:
		virtual bool CanCreateEntityFrom(const rttr::type &type) override;

		virtual void PostCreateEntity(const Ref<Asset> &asset, Ref<GameObject> &object) override;

		REFLECTABLEV(DragDropFactory)
	};

	REFLECT_EXTERN(DragDropAudio)
} // namespace BHive