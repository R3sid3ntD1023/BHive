#pragma once

#include "core/Core.h"
#include "ImWindowBase.h"

namespace BHive
{
	class World;
	class GameObject;

	class ImSceneHierarchy : public ImWindowBase
	{
	public:
		ImSceneHierarchy() = default;
		ImSceneHierarchy(const Ref<World> &world);
		~ImSceneHierarchy() = default;

		void OnUpdate();

		void SetContext(const Ref<World> &world);

		virtual const char *GetName() const override { return "SceneHeirachy"; }

	private:
		void DrawNode(GameObject *obj);

		const std::vector<rttr::type> &GetSpawnableGameobjects();

	private:
		Ref<World> mWorld;

		std::vector<rttr::type> mTypeCache;
	};
} // namespace BHive