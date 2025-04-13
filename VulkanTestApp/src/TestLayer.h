#pragma once

#include "core/Layer.h"

namespace BHive
{
	class TestLayer : public Layer
	{
	public:
		void OnAttach() override;
		void OnUpdate(float) override;
	};
} // namespace BHive