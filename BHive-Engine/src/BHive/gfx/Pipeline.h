#pragma once

#include "core/Core.h"

namespace BHive
{
	class Pipeline
	{
	public:
		struct Configuration
		{
			virtual ~Configuration() = default;
		};

		virtual ~Pipeline() = default;

		virtual void Init(const Ref<Configuration>& configuration) = 0;

		virtual void Bind() = 0;

		virtual void UnBind() = 0;

		static Ref<Pipeline> Create();
	};
} // namespace BHive