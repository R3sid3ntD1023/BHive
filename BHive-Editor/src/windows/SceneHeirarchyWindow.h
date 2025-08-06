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

		void OnUpdateContent() override;

		void SetContext(const Ref<World> &world);

		virtual const char *GetName() const override { return "SceneHeirachy"; }

	private:
		void DrawNode(GameObject *obj);

	private:
		Ref<World> mWorld;
	};
} // namespace BHive