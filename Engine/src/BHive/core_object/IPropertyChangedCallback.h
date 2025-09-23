#pragma once

#include "core/Core.h"

namespace BHive
{
	struct BHIVE_API IPropertyChangedCallback
	{
		virtual void OnPropertyChangedEvent(const rttr::property &property) {};

		virtual bool CanEditChangedProperty(const rttr::property &property) { return true; }

		REFLECTABLEV()
	};

	REFLECT_INLINE(IPropertyChangedCallback)
	{
		BEGIN_REFLECT(IPropertyChangedCallback)
		REFLECT_METHOD("OnPropertyChangedEvent", &IPropertyChangedCallback::OnPropertyChangedEvent)
		REFLECT_METHOD("CanEditChangedProperty", &IPropertyChangedCallback::CanEditChangedProperty);
	}
} // namespace BHive