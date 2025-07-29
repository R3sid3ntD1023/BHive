#pragma once

#include "Inspector.h"

namespace BHive
{
	struct Inspector_Enum : public Inspector
	{
		using EnumNames = std::unordered_map<int, std::string>;

		INSPECTOR_BODY()

		REFLECTABLEV(Inspector)

	protected:
		EnumNames &GetEnumNameValues(const rttr::enumeration &enumeration);

	private:
		static inline std::unordered_map<rttr::type::type_id, EnumNames> mEnumNameCache;
	};

	struct Inspector_EnumAsByte : public Inspector_Enum
	{

		INSPECTOR_BODY()

		REFLECTABLEV(Inspector_Enum)
	};

	REFLECT_EXTERN(Inspector_Enum)
	REFLECT_EXTERN(Inspector_EnumAsByte)
} // namespace BHive
