#pragma once

#include "core/Core.h"
#include "selection/SelectionEvents.h"

namespace BHive
{
	struct Component;
	class Entity;
	class ObjectBase;
	class Component;

	class PropertiesPanel
	{
	public:
		FGetActiveEntityEvent mGetActiveEntity;
		FGetActiveObjectEvent mGetActiveObject;
		FOnObjectSelectedEvent mOnObjectSelected;
		FOnObjectDeselectedEvent mOnObjectDeselected;

	public:
		PropertiesPanel() = default;

		void OnGuiRender();

	private:
		void DrawComponents(Entity *entity);
		void DrawComponent(Component *component);
		void DrawAddComponent(Entity *entity);

	private:
		std::unordered_set<Component *> mDeletedComponents;
	};

} // namespace BHive
