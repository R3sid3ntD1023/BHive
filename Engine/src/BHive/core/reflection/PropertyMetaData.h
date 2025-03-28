#pragma once

#include "core/Core.h"

namespace BHive
{
	enum EPropertyMetaData : uint32_t
	{
		EPropertyMetaData_None,
		EPropertyMetaData_DisplayName,
		EPropertyMetaData_Min,
		EPropertyMetaData_Max,
		EPropertyMetaData_Step,
		EPropertyMetaData_Slider,
		EPropertyMetaData_Default,
		EPropertyMetaData_HDR,
		EPropertyMetaData_CustomFormat,
		EPropertyMetaData_Flags
	};

	enum EPropertyFlags : uint32_t
	{
		EPropertyFlags_None = 0,
		EPropertyFlags_Slider = BIT(0),	   // Display arthmetic types as slider
		EPropertyFlags_FixedSize = BIT(1), // Non Editable TContainer
		EPropertyFlags_BitFlags = BIT(2)   // Display Enum as Flags
	};

	enum EClassMetaData : uint32_t
	{
		ClassMetaData_ComponentSpawnable,
		ClassMetaData_Spawnable
	};
} // namespace BHive
