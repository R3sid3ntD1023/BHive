#pragma once

#include "core/Core.h"
#include "core/EventDelegate.h"
#include "EditorLayer.h"
#include "selection/Selection.h"

namespace BHive
{
	class World;
	class ObjectBase;
	class Entity;

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
