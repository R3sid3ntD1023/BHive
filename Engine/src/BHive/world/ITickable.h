#pragma once

#include "core/Core.h"

namespace BHive
{
	struct ITickable
	{
		virtual void Update(float) {};

		void SetTickEnabled(bool enabled);

		bool IsTickEnabled() const { return mTickEnabled; }

	private:
		bool mTickEnabled = true;

		REFLECTABLEV()
	};

	REFLECT_EXTERN(ITickable)
} // namespace BHive