#pragma once

#include "core/Core.h"
#include "asset/AssetType.h"
#include "selection/SelectionEvents.h"

namespace BHive
{
	class World;
	class Entity;
	class ObjectBase;

	class SceneHierarchyPanel
	{
	public:
		FOnObjectSelectedEvent mOnEntitySelected;
		FOnObjectDeselectedEvent mOnEntityDeselected;
		FGetSelectedObjectEvent mOnGetSelectedObject;

	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<World> &world);
		~SceneHierarchyPanel() = default;

		void OnGuiRender();

		void SetContext(const Ref<World> &world);

	private:
		void DrawEntityNode(Entity *entity);

		const std::vector<AssetType> &GetSpawnableEntites();

	private:
		Ref<World> mWorld;

		std::vector<Entity *> mDestroyedentitys;

		std::vector<AssetType> mEntityTypeCache;
	};
} // namespace BHive