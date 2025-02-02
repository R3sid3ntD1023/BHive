#pragma once

#include "core/EventDelegate.h"
#include "selection/SelectionEvents.h"

namespace BHive
{
	class DetailsPanel
	{
	public:
		FGetSelectedObjectEvent mGetSelectedObject;

	public:
		void OnGuiRender();
	};

} // namespace BHive
