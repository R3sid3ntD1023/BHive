#pragma once

#include "core/Core.h"
#include "asset/AssetType.h"

namespace BHive
{
	class World;
	class Actor;



	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<World> &world);
		~SceneHierarchyPanel() = default;

		void OnGuiRender();

		void SetContext(const Ref<World> &world);

	private:
	
		void DrawActorNode(Actor *actor);

		const std::vector<AssetType>& GetSpawnableActors();

	private:
		Ref<World> mWorld;

		std::vector<Actor *> mDestroyedActors;

		std::vector<AssetType> mActorTypeCache;
	};
}