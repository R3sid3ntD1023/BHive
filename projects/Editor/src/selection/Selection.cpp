#include "Selection.h"
#include "scene/Entity.h"
#include "scene/Component.h"

namespace BHive
{
	void Selection::Select(ObjectBase *object, bool append)
	{
		if (!append)
		{
			Clear();
		}

		mSelectedObjects.insert(object);
		mActiveObject = object;

		if (auto entity = Cast<Entity>(object))
			mActiveEntity = entity;
	}

	void Selection::Deselect(ObjectBase *object)
	{
		if (mSelectedObjects.contains(object))
		{
			mSelectedObjects.erase(object);

			if (mActiveObject == object)
			{
				mActiveObject = nullptr;
			}

			if (auto entity = Cast<Entity>(object) && mActiveEntity == object)
			{
				mActiveEntity = nullptr;
			}
		}
	}

	void Selection::Clear()
	{
		mSelectedObjects.clear();
		mActiveObject = nullptr;
		mActiveEntity = nullptr;
	}

	bool Selection::IsSelected(ObjectBase *object) const
	{
		return mSelectedObjects.contains(object);
	}

} // namespace BHive