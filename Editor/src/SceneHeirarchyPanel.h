#pragma once

#include "core/Core.h"
#include "asset/AssetType.h"

namespace BHive
{
	class World;
	class Entity;



	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<World> &world);
		~SceneHierarchyPanel() = default;

		void OnGuiRender();

		void SetContext(const Ref<World> &world);

	private:
	
		void DrawEntityNode(Entity *entity);

		const std::vector<AssetType>& GetSpawnableEntites();

	private:
		Ref<World> mWorld;

		std::vector<Entity *> mDestroyedentitys;

		std::vector<AssetType> mentityTypeCache;
	};
}