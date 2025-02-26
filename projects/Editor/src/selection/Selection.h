#pragma once

#include "core/Core.h"

namespace BHive
{
	class ObjectBase;
	class Entity;

	struct Selection
	{
		using SelectionContainer = std::unordered_set<ObjectBase *>;

		void Select(ObjectBase *object, bool append = false);
		void Deselect(ObjectBase *object);
		void Clear();
		bool IsSelected(ObjectBase *object) const;

		const SelectionContainer &GetSelectedObjects() const { return mSelectedObjects; }
		ObjectBase *GetActiveObject() const { return mActiveObject; }
		Entity *GetActiveEntity() const { return mActiveEntity; }

		operator bool() const { return mSelectedObjects.size(); }

	private:
		ObjectBase *mActiveObject = nullptr;
		Entity *mActiveEntity = nullptr;
		SelectionContainer mSelectedObjects;
	};
} // namespace BHive