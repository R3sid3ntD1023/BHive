#include "InspectorDelegate.h"
#include "RuntimeEventDelegate.h"
#include "Inspectors.h"
#include "objects/GameObject.h"

namespace BHive
{
	bool Inspector_Delegate::Inspect(
		const rttr::variant &instance, rttr::variant &var, bool read_only, const meta_getter &getMetaData)
	{
		bool changed = false;
		auto data = var.get_value<RuntimeEventDelegate>();

		rttr::variant object_var = data.GetCaller();
		if (inspect(instance, object_var, false, read_only, getMetaData))
		{
			auto object = object_var.get_value<GameObject *>();
			data.SetCaller(object);
			changed |= true;
		}

		if (changed)
		{
			var = data;
		}

		return changed;
	}

	REFLECT_INSPECTOR(Inspector_Delegate, RuntimeEventDelegate)
} // namespace BHive
