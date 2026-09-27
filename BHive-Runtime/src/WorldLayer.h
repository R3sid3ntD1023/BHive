#pragma once

#include "core/Layer.h"
#include "gfx/factories/GFXFactories.h"

namespace BHive
{
	class PerformanceLayer : public Layer
	{
	public:
		void OnAttach(Application &app) override;

		void OnUpdate(float dt) override;

		void OnGuiRender() override;
	};
} // namespace BHive