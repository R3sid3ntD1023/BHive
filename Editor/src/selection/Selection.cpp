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

		auto handle = object->OnDestroyedEvent.bind([=](ObjectBase *obj) { Deselect(obj); });
		mHandles.emplace(object, handle);

		mSelectedObjects.insert(object);
		mActiveObject = object;

		if (auto entity = Cast<Entity>(object))
			mActiveEntity = entity;
	}

	void Selection::Deselect(ObjectBase *object)
	{
		if (mSelectedObjects.contains(object))
		{
			auto handle = mHandles.at(object);
			object->OnDestroyedEvent.unbind(handle);
			mHandles.erase(object);

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
		for (auto object : mSelectedObjects)
		{
			auto handle = mHandles.at(object);
			object->OnDestroyedEvent.unbind(handle);
			mHandles.erase(object);
		}

		mSelectedObjects.clear();
		mActiveObject = nullptr;
		mActiveEntity = nullptr;
	}

	bool Selection::IsSelected(ObjectBase *object) const
	{
		return mSelectedObjects.contains(object);
	}

} // namespace BHive