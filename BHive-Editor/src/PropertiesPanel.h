#pragma once

#include "core/Core.h"
#include "core/EventDelegate.h"
#include "WindowBase.h"

namespace BHive
{
	struct Component;
	struct GameObject;

	class PropertiesPanel : public WindowBase
	{
	public:
		PropertiesPanel() = default;

		void OnGuiRender();

		virtual const char *GetName() const { return "Properties"; }

	private:
		void DrawComponents(GameObject *entity);
		void DrawComponent(Component *component);
		void DrawAddComponent(GameObject *entity);

		std::vector<rttr::method> mDeleteComponents;
	};

} // namespace BHive
