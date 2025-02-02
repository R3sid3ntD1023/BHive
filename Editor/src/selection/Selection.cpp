#include "Selection.h"
#include "scene/Entity.h"
#include "scene/Component.h"

namespace BHive
{
	void Selection::Select(ObjectBase *object)
	{
		if (auto entity = Cast<Entity>(object))
		{
			mSelectedEntity = entity;
		}

		mSelectedObject = object;
	}

	void Selection::Deselect(ObjectBase *object, EDeselectReason reason)
	{
		if (auto entity = Cast<Entity>(object))
		{
			if (mSelectedEntity == entity)
				mSelectedEntity = nullptr;

			if (reason == DeselectReason_Destroyed)
			{
				if (auto component = Cast<Component>(mSelectedObject))
				{
					if (component->GetOwner() == entity)
						mSelectedObject = nullptr;
				}
			}
		}

		if (mSelectedObject == object)
			mSelectedObject = nullptr;
	}

	void Selection::Clear()
	{
		mSelectedEntity = nullptr;
		mSelectedObject = nullptr;
	}
} // namespace BHive