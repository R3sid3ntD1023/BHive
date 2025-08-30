#pragma once

#include "core/subsystem/SubSystem.h"
#include "Inspector.h"
#include "undoredo/Commands.h"
#include "undoredo/UndoRedo.h"

namespace BHive
{

	struct BHIVE_API Inspect
	{
		static rttr::variant meta_data_empty(const rttr::variant &);

		bool inspect(const rttr::variant &instance, rttr::variant &var, bool skip_custom = false, bool read_only = false, float width = 0.0f, const MetaGetter &get_meta_data = meta_data_empty);

		bool inspect(const rttr::variant &instance, rttr::variant &object, rttr::property &property, bool read_only = false, float width = 0.0f);

		// inspect readonly data
		template <typename T, typename U>
		bool inspect(const std::string &label, U owning_object, const T &obj, bool skip_custom = false, bool read_only = false, float width = 0.0f, const MetaGetter &get_meta_data = meta_data_empty)
		{
			ScopedPropertyLayout layout(label, width);
			rttr::variant var = obj;
			return inspect(owning_object, var, skip_custom, true, width, get_meta_data);
		}

		// inspect data read_only optional
		template <typename T, typename U>
		bool inspect(const std::string &label, U owning_object, T &obj, bool skip_custom = false, bool read_only = false, float width = 0.0f, const MetaGetter &get_meta_data = meta_data_empty)
		{
			ScopedPropertyLayout layout(label, width);
			rttr::variant var = obj;
			if (inspect(owning_object, var, skip_custom, read_only, width, get_meta_data))
			{

				obj = var.get_value<T>();
				return true;
			}

			return false;
		}

		static Inspect &get()
		{
			static Inspect instance;
			return instance;
		}

	private:
		rttr::type get_instance_type(const rttr::instance &object);
	};

} // namespace BHive