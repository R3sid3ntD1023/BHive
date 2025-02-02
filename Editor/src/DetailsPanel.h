#pragma once

#include "core/EventDelegate.h"
#include "selection/SelectionEvents.h"

namespace BHive
{
	class DetailsPanel
	{
	public:
		FGetActiveObjectEvent mGetActiveObject;

	public:
		void OnGuiRender();
	};

} // namespace BHive
