#pragma once

#include "core/Core.h"

namespace BHive
{
	struct GameObject;

	struct Component
	{

		virtual void Begin() {};
		virtual void Update(float) {};
		virtual void End() {};

		GameObject *GetOwner() const { return mOwningObject; }

	private:
		GameObject *mOwningObject = nullptr;
		friend struct GameObject;
	};

} // namespace BHive