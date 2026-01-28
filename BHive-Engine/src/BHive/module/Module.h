#pragma once

#include "core/Core.h"

namespace BHive
{
	class BHIVE_API Module
	{
	public:
		virtual void OnInitialize() {};

		virtual void OnShutdown() {};
	};
} // namespace BHive