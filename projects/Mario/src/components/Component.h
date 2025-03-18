#pragma once

#include "core/Core.h"
#include "core/reflection/Reflection.h"

#define ADD_COMPONENT_FUNCTION_NAME "AddComponent"

namespace BHive
{
	struct GameObject;

	struct Component
	{

		virtual void Begin() {};
		virtual void Update(float) {};
		virtual void Render() {}
		virtual void End() {};

		GameObject *GetOwner() const { return mOwningObject; }

		REFLECTABLEV()

	private:
		GameObject *mOwningObject = nullptr;
		friend struct GameObject;
	};

	REFLECT(Component)
	{
		BEGIN_REFLECT(Component);
	}

} // namespace BHive