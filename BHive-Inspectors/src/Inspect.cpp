#include "Inspect.h"
#include "PropertyLayout.h"
#include <imgui.h>

#include "AssociativeContainer.h"
#include "InspectorAsset.h"
#include "InspectorBaseTypes.h"
#include "InspectorComponent.h"
#include "InspectorCoreTypes.h"
#include "InspectorEnum.h"
#include "InspectorGameObject.h"
#include "InspectorPath.h"
#include "InspectorSubClassOf.h"
#include "InspectorType.h"
#include "Inspector_Blackboard.h"
#include "SequentialContainer.h"
#include "asset_inspectors/asset_inspectors.h"

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
				auto inspector = type.create().get_value<Inspector *>();

				if (!inspector)
					continue;

				mRegisteredInspectors.emplace(inspected_type.get_id(), Scope<Inspector>(inspector));
			}
		}

		Inspector *get_inspector(const rttr::type &type) const
		{
			if (!type)
				return nullptr;

			auto inspector = mRegisteredInspectors.find(type.get_id());
			if (inspector != mRegisteredInspectors.end())
			{
				return inspector->second.get();
			}

			for (auto base : type.get_base_classes())
			{
				if (auto base_inspector = get_inspector(base))
					return base_inspector;
			}

			if (type.is_wrapper())
			{
				auto wrapped_type = type.get_wrapped_type();
				if (auto wrapped_inspector = get_inspector(wrapped_type))
					return wrapped_inspector;

				if (auto raw_inspector = get_inspector(wrapped_type.get_raw_type()))
					return raw_inspector;
			}

			if (type.is_enumeration())
			{
				return get_inspector(rttr::type::get<rttr::enumeration>());
			}

			if (type.is_associative_container())
				return get_inspector(rttr::type::get<rttr::variant_associative_view>());

			if (type.is_sequential_container())
				return get_inspector(rttr::type::get<rttr::variant_sequential_view>());

			return nullptr;
		}

		static InspectorRegistry &get()
		{
			static InspectorRegistry registry;
			return registry;
		}

	private:
		std::unordered_map<rttr::type::type_id, Scope<Inspector>> mRegisteredInspectors;
	};

	rttr::type Inspect::get_instance_type(const rttr::instance &object)
	{
		rttr::type _type = rttr::type::get<rttr::detail::invalid_type>();

		_type = object.get_derived_type();
		if (!_type)
		{
			_type = object.get_type();
		}

		return _type;
	}

	rttr::variant Inspect::meta_data_empty(const rttr::variant &)
	{
		return rttr::variant();
	}

	bool Inspect::needs_tree_node(const rttr::variant &var)
	{
		auto type = get_instance_type(var);
		return type && (!type.get_properties().empty() || type.is_associative_container() || type.is_sequential_container());
	}

	bool Inspect::inspect(const rttr::variant &instance, rttr::variant &var, bool skip_custom, bool read_only, float width, const MetaGetter &get_meta_data)
	{
		rttr::instance object = var;
		auto type = get_instance_type(object);
		auto inspector = InspectorRegistry::get().get_inspector(type);

		if (!inspector)
		{
			if (type.is_wrapper())
			{
				type = object.get_wrapped_instance().get_derived_type();
			}
		}

		auto properties = type.get_properties();
		bool changed = false;

		if (!skip_custom && inspector)
		{
			return inspector->inspect(instance, var, get_meta_data, read_only);
		}

		if (object)
		{
			for (auto property : properties)
			{

				changed |= inspect(instance, var, property, read_only, width);
			}
		}

		return changed;
	}

	bool Inspect::inspect(const rttr::variant &instance, rttr::variant &object, rttr::property &property, bool read_only, float width)
	{
		rttr::variant prop_var = property.get_value(object);
		rttr::variant original_var = prop_var;
		rttr::instance prop_object = prop_var;
		auto type = get_instance_type(prop_object);
		auto inspector = InspectorRegistry::get().get_inspector(type);
		bool is_read_only = property.is_readonly() || read_only;
		bool is_enum = property.is_enumeration();
		bool is_container = type.is_associative_container() || type.is_sequential_container();
		bool has_properties = !type.get_properties().empty();
		bool details = !is_enum && (has_properties || is_container);

		bool opened = true;

		if (details)
		{
			opened = ImGui::TreeNodeEx(property.get_name().data(), ImGuiTreeNodeFlags_SpanAvailWidth);
		}

		bool changed = false;
		if (opened)
		{
			if (inspector)
			{
				inspector->begin_inspect(property, !is_container, width);
			}

			/*PropertyLayout layout(property, true, width);
			layout.PushLayout();*/

			auto meta_getter = [property](const rttr::variant &key) -> rttr::variant { return property.get_metadata(key); };
			changed |= inspect(instance, prop_var, false, is_read_only, width, meta_getter);

			/*	layout.PopLayout();*/

			if (inspector)
			{
				inspector->end_inspect(property);
			}

			if (changed && !is_read_only)
			{
				// GetSubSystem<UndoRedo>().AddCommand<CommandProperty>(std::format("Changed Property {}", property.get_name().data()), object, property, prop_var);
				property.set_value(object, prop_var);
			}
		}

		if (details && opened)
		{
			ImGui::TreePop();
		}

		return changed;
	}
} // namespace BHive
