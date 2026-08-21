#pragma once

#include "core/Core.h"
#include "NativeHandle.h"

namespace BHive
{
	class BHIVE_API Query
	{
	public:
		virtual ~Query() = default;

		virtual std::vector<float> GetResults() const = 0;

		virtual bool IsResultsReady() const = 0;

		static Ref<Query> Create(uint32_t count);
	};

} // namespace BHive
