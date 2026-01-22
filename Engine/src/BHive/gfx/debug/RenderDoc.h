#pragma once

#include "core/Core.h"

namespace BHive
{
	class BHIVE_API APIDebugger
	{
	public:
		virtual ~APIDebugger() = default;

		virtual void Init() {};

		static Ref<APIDebugger> Create();
	};
} // namespace BHive