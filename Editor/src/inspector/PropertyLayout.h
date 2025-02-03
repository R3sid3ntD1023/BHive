#pragma once

#include "core/reflection/Reflection.h"

#define PROPERTY_PRETTY_NAME "PrettyName"

namespace BHive
{
	struct PropertyLayout
	{
		PropertyLayout() = default;
		PropertyLayout(const std::string &name, bool columns = true, float width = 0.0f);
		PropertyLayout(const rttr::property &property, bool columns = true, float width = 0.0f);
		~PropertyLayout();

		void PushLayout();
		void PopLayout();

	private:
		float mWidth{0.0f};
		bool mColumns{true};
		std::string mName = "";
	};

	struct ScopedPropertyLayout : public PropertyLayout
	{
		ScopedPropertyLayout(const std::string &name, bool columns = true, float width = 0.0f);
		ScopedPropertyLayout(const rttr::property &property, bool columns = true, float width = 0.0f);
		~ScopedPropertyLayout();
	};
} // namespace BHive
