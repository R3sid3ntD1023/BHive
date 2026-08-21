#pragma once

#include "core/Layer.h"

namespace BHive
{
	class PerformanceLayer : public Layer
	{
	public:
		void OnGuiRender() override;
	};
} // namespace BHive