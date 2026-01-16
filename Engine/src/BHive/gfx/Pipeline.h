#pragma once

#include "core/Core.h"

namespace BHive
{
	class Pipeline
	{
	public:
		struct Configuration
		{
		};

		virtual ~Pipeline() = default;

		virtual void Init() = 0;

		virtual void Bind() = 0;

		virtual void UnBind() = 0;

		static Ref<Pipeline> Create(const Configuration &configuration);
	};
} // namespace BHive