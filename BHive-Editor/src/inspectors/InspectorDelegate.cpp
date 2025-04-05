#include "core/EventDelegate.h"
#include "core/EventDelegate.h"
#include "GameObject.h"
#include "InspectorDelegate.h"
#include "Inspectors.h"

namespace rttr
{
	/*template <typename TRet, typename... TArgs>
	struct wrapper_mapper<BHive::EventDelegateBase<TRet, TArgs...>>
	{

		using type = BHive::EventDelegateBase<TRet, TArgs...>;
		using wrapped_type = type::Function;

		inline static type get(const wrapped_type &value) { return value; }
	};*/
} // namespace rttr

namespace BHive
{

	/*bool Inspector_Delegate::Inspect(
		const rttr::variant &instance, rttr::variant &var, bool read_only, const meta_getter &getMetaData)
	{
		bool changed = false;
		auto data = var.extract_wrapped_value().get_value<EventDelegateBase*>();

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

	REFLECT_INSPECTOR(Inspector_Delegate, RuntimeEventDelegate)*/
} // namespace BHive
