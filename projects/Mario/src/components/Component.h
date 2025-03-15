#pragma once

#include "core/Core.h"

namespace BHive
{
	struct GameObject;

	struct Component
	{
		Component(GameObject *owner)
			: mOwningObject(owner)
		{
		}

		virtual void Begin() {};
		virtual void Update(float) {};
		virtual void End() {};

		GameObject *GetOwner() const { return mOwningObject; }

	private:
		GameObject *mOwningObject = nullptr;
	};

} // namespace BHive