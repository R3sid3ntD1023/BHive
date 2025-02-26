#pragma once

#include "core/EventDelegate.h"

namespace BHive
{
	class Entity;
	class ObjectBase;

	DECLARE_RET_EVENT(FGetActiveObject, ObjectBase *);
	DECLARE_RET_EVENT(FGetActiveEntity, Entity *);
	DECLARE_RET_EVENT(FIsSelected, bool, ObjectBase *);
	DECLARE_EVENT(FOnObjectDeselected, ObjectBase *);
	DECLARE_EVENT(FOnObjectSelected, ObjectBase *, bool);
	DECLARE_EVENT(FOnClearSelection);
} // namespace BHive