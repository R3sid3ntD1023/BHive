#include "core/Core.h"
#include "gui/ImGuiExtended.h"
#include "Inspectors.h"
#include "PropertyLayout.h"

namespace BHive
{
	struct InspectorRegistry
	{
		InspectorRegistry()
		{
			for (auto type : rttr::type::get<Inspector>().get_derived_classes())
			{
				auto inspected_type_var = type.get_metadata(INSPECTED_TYPE_VAR);
				if (!inspected_type_var)
					continue;

				auto inspected_type = inspected_type_var.get_value<rttr::type>();
				auto inspector = type.create();

				if (!inspector)
					continue;

				mRegisteredInspectors.emplace(inspected_type, inspector.get_value<Ref<Inspector>>());
			}
		}

		Ref<Inspector> GetInspector(const rttr::type &type) const
		{
			Ref<Inspector> inspector = nullptr;

			if (mRegisteredInspectors.contains(type))
			{
				inspector = mRegisteredInspectors.at(type);
			}

			if (!inspector)
			{
				for (auto base : type.get_base_classes())
				{
					if (mRegisteredInspectors.contains(base))
					{
						inspector = mRegisteredInspectors.at(base);
					}
				}
			}

			if (!inspector && type.is_wrapper())
			{
				auto wrapped_type = type.get_wrapped_type();
				inspector = GetInspector(wrapped_type);

				if (!inspector)
				{
					auto raw_type = wrapped_type.get_raw_type();
					inspector = GetInspector(raw_type);
				}
			}
			
			if (type.is_enumeration())
			{
				return mRegisteredInspectors.at(rttr::type::get<rttr::enumeration>());
			}

			if (type.is_associative_container())
			{
				return mRegisteredInspectors.at(rttr::type::get<rttr::variant_associative_view>());
			}

			if (type.is_sequential_container())
			{
				return mRegisteredInspectors.at(rttr::type::get<rttr::variant_sequential_view>());
			}

			return inspector;
		}

		static InspectorRegistry Get()
		{
			static InspectorRegistry registry;
			return registry;
		}

	private:
		std::unordered_map<rttr::type, Ref<Inspector>> mRegisteredInspectors;
	};

	void SetCurrentContext(ImGuiContext *ctx)
	{
		ImGui::SetCurrentContext(ctx);
	}

	rttr::type GetInstanceType(const rttr::instance &object)
	{
		rttr::type _type = rttr::type::get<rttr::detail::invalid_type>();

		_type = object.get_derived_type();
		if (!_type)
		{
			_type = object.get_type();
		}

		// if (_type.is_wrapper())
		// {
		// 	_type = object.get_wrapped_instance().get_derived_type();

		// 	if (!_type)
		// 	{
		// 		_type = object.get_wrapped_instance().get_type();
		// 	}
		// }
		// else
		// {
		// 	_type = object.get_derived_type();
		// 	if (!_type)
		// 	{
		// 		_type = object.get_type();
		// 	}
		// }

		return _type;
	}

	rttr::variant meta_data_empty(const rttr::variant &)
	{
		return rttr::variant();
	}

	bool inspect(
		const rttr::variant &instance, rttr::variant &var, bool skip_custom, bool read_only,
		const Inspector::meta_getter &get_meta_data, float width)
	{
		rttr::instance object = var;
		auto type = GetInstanceType(object);
		auto inspector = InspectorRegistry::Get().GetInspector(type);

		auto properties = type.get_properties();
		bool changed = false;

		if (!skip_custom && inspector)
		{
			changed |= inspector->Inspect(instance, var, read_only, get_meta_data);
		}
		else
		{

			for (auto property : properties)
			{

				changed |= inspect(var, object, property, read_only, width);
			}
		}

		return changed;
	}

	bool
	inspect(const rttr::variant &instance, rttr::instance &object, rttr::property &property, bool read_only, float width)
	{
		rttr::variant prop_var = property.get_value(object);
		rttr::instance prop_object = prop_var;
		auto type = GetInstanceType(prop_object);
		auto inspector = InspectorRegistry::Get().GetInspector(type);
		bool is_read_only = property.is_readonly() || read_only;
		bool changed = false;
		bool has_inspector = inspector != nullptr;
		bool is_enum = property.is_enumeration();
		bool details = !has_inspector && !is_enum;
		bool is_container = type.is_associative_container() || type.is_sequential_container();

		if (inspector)
		{
			inspector->BeginInspect(property, !is_container, width);
		}

		bool opened = true;

		if (details)
		{
			opened = ImGui::TreeNode(property.get_name().data());
		}

		if (opened)
		{

			PropertyLayout layout(property);
			auto meta_getter = [property](const rttr::variant &key) -> rttr::variant { return property.get_metadata(key); };
			changed |= inspect(instance, prop_var, false, is_read_only, meta_getter, width);
		}

		if (details)
		{
			if (opened)
			{
				ImGui::TreePop();
			}
		}

		if (changed && !is_read_only)
		{
			property.set_value(object, prop_var);
		}

		if (inspector)
		{
			inspector->EndInspect(property);
		}

		return changed;
	}
} // namespace BHive
