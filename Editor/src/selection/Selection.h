#pragma once

namespace BHive
{
	class ObjectBase;
	class Entity;

	enum EDeselectReason
	{
		DeselectReason_None,
		DeselectReason_Destroyed
	};

	struct Selection
	{

		void Select(ObjectBase *object);
		void Deselect(ObjectBase *object, EDeselectReason reason = DeselectReason_None);
		void Clear();

		ObjectBase *GetSelectedObject() const { return mSelectedObject; }
		Entity *GetSelectedEntity() const { return mSelectedEntity; }

		bool operator==(const ObjectBase *rhs) const { return mSelectedObject == rhs; }
		operator bool() const { return mSelectedObject; }

	private:
		ObjectBase *mSelectedObject = nullptr;
		Entity *mSelectedEntity = nullptr;
	};
} // namespace BHive