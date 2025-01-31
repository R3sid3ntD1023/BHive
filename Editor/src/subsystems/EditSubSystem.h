#pragma once

#include "core/Core.h"
#include "core/EventDelegate.h"
#include "EditorLayer.h"

namespace BHive
{
	class World;
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

	private:
		ObjectBase *mSelectedObject = nullptr;
		Entity *mSelectedEntity = nullptr;
	};

	DECLARE_RET_EVENT(OnGetEditorMode, EEditorMode);
	DECLARE_RET_EVENT(OnGetActiveWorld, Ref<World>);

	struct EditSubSystem
	{
		void OnUpdate(float dt);

		OnGetEditorModeEvent GetEditorMode;
		OnGetActiveWorldEvent GetActiveWorld;

		Selection mSelection;
	};

} // namespace BHive
