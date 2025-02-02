#pragma once

#include "core/EventDelegate.h"

namespace BHive
{
	class Entity;
	class ObjectBase;

	DECLARE_RET_EVENT(FGetSelectedEntity, Entity *);
	DECLARE_RET_EVENT(FGetSelectedObject, ObjectBase *);
	DECLARE_EVENT(FOnObjectDeselected, ObjectBase *, uint8_t);
	DECLARE_EVENT(FOnObjectSelected, ObjectBase *);
} // namespace BHive