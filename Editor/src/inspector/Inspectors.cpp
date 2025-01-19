#include "core/Core.h"
#include "gui/ImGuiExtended.h"
#include "inspector/Inspectors.h"
#include "inspector/PropertyLayout.h"

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

				mRegisteredInspectors.emplace(
					inspected_type, inspector.get_value<Ref<Inspector>>());
			}
		}

		Ref<Inspector> GetInspector(const rttr::type &type) const
		{
			rttr::type current_type = type;

			// if(current_type.is_wrapper())
			//     current_type = current_type.get_wrapped_type().get_raw_type();

			if (mRegisteredInspectors.contains(current_type))
			{
				return mRegisteredInspectors.at(current_type);
			}

			for (auto base : current_type.get_base_classes())
			{
				if (mRegisteredInspectors.contains(base))
				{
					return mRegisteredInspectors.at(base);
				}
			}

			if (current_type.is_enumeration())
			{
				return mRegisteredInspectors.at(rttr::type::get<rttr::enumeration>());
			}

			if (current_type.is_associative_container())
			{
				return mRegisteredInspectors.at(rttr::type::get<rttr::variant_associative_view>());
			}

			if (current_type.is_sequential_container())
			{
				return mRegisteredInspectors.at(rttr::type::get<rttr::variant_sequential_view>());
			}

			return nullptr;
		}

		static InspectorRegistry Get()
		{
			static InspectorRegistry registry;
			return registry;
		}

	private:
		std::unordered_map<rttr::type, Ref<Inspector>> mRegisteredInspectors;
	};

	rttr::variant meta_data_empty(const rttr::variant &)
	{
		return rttr::variant();
	}

	bool inspect(
		rttr::variant &var, bool skip_custom, bool read_only,
		const Inspector::meta_getter &get_meta_data)
	{
		rttr::instance object = var;
		auto type = object.get_derived_type();
		auto inspector = InspectorRegistry::Get().GetInspector(type);
		auto properties = type.get_properties();
		bool changed = false;

		if (!skip_custom && inspector)
		{
			changed |= inspector->Inspect(var, read_only, get_meta_data);
		}
		else
		{

			for (auto property : properties)
			{

				changed |= inspect(object, property, read_only);
			}
		}

		return changed;
	}

	bool inspect(rttr::instance &object, rttr::property &property, bool read_only)
	{
		rttr::variant prop_var = property.get_value(object);
		rttr::instance prop_object = prop_var;
		auto type = prop_object.get_derived_type();
		auto inspector = InspectorRegistry::Get().GetInspector(type);
		bool is_read_only = property.is_readonly() || read_only;
		bool changed = false;
		bool has_inspector = inspector != nullptr;
		bool is_enum = property.is_enumeration();
		bool details = !has_inspector && !is_enum;
		bool is_container = type.is_associative_container() || type.is_sequential_container();

		if (inspector)
		{
			inspector->BeginInspect(property, !is_container);
		}

		bool opened = true;
		
		if (details)
		{
			opened = ImGui::TreeNode(property.get_name().data());
		}

		if (opened)
		{

			PropertyLayout layout(property);
			auto meta_getter = [property](const rttr::variant &key) -> rttr::variant
			{ return property.get_metadata(key); };
			changed |= inspect(prop_var, false, is_read_only, meta_getter);

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
