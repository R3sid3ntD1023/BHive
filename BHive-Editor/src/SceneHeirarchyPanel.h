#pragma once

#include "core/Core.h"
#include "WindowBase.h"

namespace BHive
{
	class World;
	struct GameObject;

	class SceneHierarchyPanel : public WindowBase
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<World> &world);
		~SceneHierarchyPanel() = default;

		void OnGuiRender();

		void SetContext(const Ref<World> &world);

		virtual const char *GetName() const { return "SceneHeirachy"; }

	private:
		void DrawNode(GameObject *obj);

		const std::vector<rttr::type> &GetSpawnableEntites();

	private:
		Ref<World> mWorld;

		std::vector<rttr::type> mEntityTypeCache;
	};
} // namespace BHive