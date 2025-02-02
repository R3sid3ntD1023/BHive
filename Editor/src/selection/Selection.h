#pragma once

#include "core/Core.h"

namespace BHive
{
	class ObjectBase;
	class Entity;

	struct Selection
	{
		using SelectionContainer = std::unordered_set<ObjectBase *>;
		using SelectionHandleContainer = std::unordered_map<ObjectBase *, EventDelegateHandle>;

		void Select(ObjectBase *object, bool append = false);
		void Deselect(ObjectBase *object);
		void Clear();
		bool IsSelected(ObjectBase *object) const;

		const SelectionContainer &GetSelectedObjects() const { return mSelectedObjects; }
		ObjectBase *GetActiveObject() const { return mActiveObject; }

		operator bool() const { return mSelectedObjects.size(); }

	private:
		ObjectBase *mActiveObject = nullptr;
		SelectionContainer mSelectedObjects;
		SelectionHandleContainer mHandles;
	};
} // namespace BHive